#include "application.h"

#include "input.h"
#include "selection/base_context.h"
#include "imgui/module.h"

#include "egfx/scene.h"
#include "egfx/object/camera.h"
#include "egfx/node/light.h"

#include "Ogre/ConfigOptionMap.h"
#include "Ogre/RenderSystem.h"

#include "SDL2/window.h"
#include "SDL2/macro.h"
#include <SDL_video.h>

#include <imgui_impl_sdl.h>
#include <ImGuizmo.h>

#include <iterator>

namespace ot::dedit
{
	namespace
	{
		size_t get_number_threads()
		{
			return Ogre::PlatformInformation::getNumLogicalCores();
		}

		void push_window_event(SDL_Event const& e, std::vector<egfx::window_event>& window_events)
		{
			using egfx::window_event;
			using egfx::window_id;
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_MOVED:
					window_events.push_back({ window_id(e.window.windowID), window_event::moved{e.window.data1, e.window.data2} });
					break;
				case SDL_WINDOWEVENT_RESIZED:
					window_events.push_back({ window_id(e.window.windowID), window_event::resized{e.window.data1, e.window.data2} });
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					window_events.push_back({ window_id(e.window.windowID), window_event::focus_gained{} });
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					window_events.push_back({ window_id(e.window.windowID), window_event::focus_lost{} });
					break;
				}
				break;
			}
		}
	}

	application::application(sdl::unique_window window, egfx::module& graphics, config const& program_config)
		: main_window(std::move(window))
		, graphics(graphics)
		, main_scene(graphics.create_scene(std::string(program_config.get_scene().get_workspace()), get_number_threads() - 1))
		, current_map(main_scene.get_root_node())
	{
		if (auto const maybe_ambiant = program_config.get_scene().get_ambient_light())
		{
			auto const& ambiant_light = *maybe_ambiant;
			main_scene.set_ambiant_light(ambiant_light.upper_hemisphere, ambiant_light.upper_hemisphere, ambiant_light.hemisphere_direction);
		}

		auto const& render_window = graphics.get_window(egfx::window_id{ SDL_GetWindowID(main_window.get()) });
		selection_context.reset(new selection::base_context(current_map, main_scene, render_window));

		egfx::object::camera_ref const camera = main_scene.get_camera();
		camera.set_position({ 0.0f, 2.0f, -5.5f });
		camera.look_at({ 0.0f, 0.0f, 0.0f });

		light = egfx::node::create_directional_light(main_scene.get_root_node());
		light.set_position({ 10.0f, 10.0f, -10.0f });
		light.set_direction(normalized(math::vector3f{ -1.0f, -1.0f, 1.0f }));

		selection_render = egfx::node::create_manual(main_scene.get_root_node());
	}

	void application::run()
	{
		using namespace ot::math::literals;
		auto last_frame = std::chrono::steady_clock::now();
		auto current_frame = last_frame;

		while (!wants_quit) 
		{
			start_frame();
		
			handle_events();

			update(current_frame - last_frame);

			if (!render())
			{
				wants_quit = true;
			}

			end_frame();

			last_frame = std::exchange(current_frame, std::chrono::steady_clock::now());
		}
	}

	void application::start_frame()
	{
		mouse.start_frame();
		graphics.start_frame();
		selection_context->start_frame();
	}

	void application::handle_events()
	{
		SDL_PumpEvents();

		ImGuiIO& imgui_io = ImGui::GetIO();
		std::vector<egfx::window_event> window_events;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_KEYUP:
			case SDL_KEYDOWN:
			{
				SDL_KeyboardEvent const& key = e.key;
				auto const modifiers = input::keyboard::get_modifiers();

				ImGui_ImplSDL2_ProcessEvent(&e);

				if (imgui_io.WantCaptureKeyboard)
				{
					if (key.state == SDL_PRESSED && key.keysym.scancode == SDL_SCANCODE_F4 && modifiers == input::keyboard::mod::lalt)
						wants_quit = true;

					break;
				}

				// Ignore keyboard while using ImGuizmo
				if (ImGuizmo::IsUsing())
					break;

				if (camera_controller::handle_keyboard_event(e.key))
					break;

				if (selection_actions.handle_keyboard_event(e.key, current_map))
					break;

				if (menu::handle_keyboard_event(e.key))
					break;

				if (selection_context != nullptr && selection_context->handle_keyboard_event(key, selection_actions))
					break;

				break;
			}
			case SDL_TEXTINPUT:
				ImGui_ImplSDL2_ProcessEvent(&e);
				break;

			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				ImGui_ImplSDL2_ProcessEvent(&e);

				if (imgui::has_mouse())	
					break;
				
				if (camera_controller::handle_mouse_button_event(e.button))
					break;

				if (mouse.handle_mouse_button_event(e.button))
					break;

				break;

			case SDL_MOUSEWHEEL:
				ImGui_ImplSDL2_ProcessEvent(&e);
				
				if (imgui::has_mouse())
					break;

				break;

			case SDL_MOUSEMOTION:
				if (camera_controller::handle_mouse_motion_event(e.motion))
					break;

				if (mouse.handle_mouse_motion_event(e.motion))
					break;

				break;

			case SDL_WINDOWEVENT:
				ImGui_ImplSDL2_ProcessEvent(&e);
				push_window_event(e, window_events);

				if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(main_window.get()))
				{
					wants_quit = true;
				}

				break;

			case SDL_QUIT:
				wants_quit = true;
				break;
			}
		}

		graphics.on_window_events(window_events);

		imgui::new_frame(*main_window);
	}

	void application::update(math::seconds dt)
	{
		menu::update();

		camera_controller::update(dt);

		input::frame_input input;
		input.mouse_action = mouse.get_action();

		selection_render.clear();
		selection_context->update(selection_render, selection_actions, input);

		selection_actions.apply_actions(current_map);
	}

	bool application::render()
	{
		// Render
		imgui::render();

		// End frame
		if (!graphics.render())
		{
			return false;
		}		

		return true;
	}

	void application::end_frame()
	{
		imgui::end_frame();
	}
}