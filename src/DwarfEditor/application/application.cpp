#include "application.h"

#include "input.h"
#include "selection/base_context.h"
#include "imgui/module.h"

#include "egfx/scene.h"
#include "egfx/object/camera.h"
#include "egfx/node/mesh.h"
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
		math::plane const cube_planes[6] = {
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		};

		auto const sqrt_half = 0.70710678118654752440084436210485f;
		math::plane const octagon_planes[] = {
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{1, 0, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, 0, 1}, 0.5},
			{{0, 0, -1}, 0.5},
			{{sqrt_half, 0, sqrt_half}, 0.5},
			{{sqrt_half, 0, -sqrt_half}, 0.5},
			{{-sqrt_half, 0, sqrt_half}, 0.5},
			{{-sqrt_half, 0, -sqrt_half}, 0.5},
		};

		math::plane const pyramid_planes[] = {
			{{0, -1, 0}, 0.5},
			{{sqrt_half, sqrt_half, 0}, 0.5},
			{{-sqrt_half, sqrt_half, 0}, 0.5},
			{{0, sqrt_half, sqrt_half}, 0.5},
			{{0, sqrt_half, -sqrt_half}, 0.5},
		};

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

	application::application(sdl::unique_window window, egfx::module& graphics)
		: main_window(std::move(window))
		, graphics(graphics)
	{

	}

	bool application::initialize(config const& program_config)
	{
		main_scene = graphics.create_scene(std::string(program_config.get_scene().get_workspace()), get_number_threads() - 1);
		
		if (auto const maybe_ambiant = program_config.get_scene().get_ambient_light())
		{
			auto const ambiant_light = *maybe_ambiant;
			main_scene.set_ambiant_light(ambiant_light.upper_hemisphere, ambiant_light.upper_hemisphere, ambiant_light.hemisphere_direction);
		}

		auto const& render_window = graphics.get_window(egfx::window_id{ SDL_GetWindowID(main_window.get()) });
		selection_context.reset(new selection::base_context(current_map, main_scene, render_window));

		return true;
	}
	 
	void application::setup_default_scene()
	{
		current_map.add_brush(make_brush(cube_planes, "Cube", { 2.5f, 0.0f, 0.0f }));
		current_map.add_brush(make_brush(octagon_planes, "Octagon", { 0.0f, 0.0f, 0.0f }));
		current_map.add_brush(make_brush(pyramid_planes, "Pyramid", { -2.5f, 0.0f, 0.0f }));

		egfx::object::camera_ref const camera = main_scene.get_camera();
		camera.set_position({ 0.0f, 2.0f, -5.5f });
		camera.look_at({ 0.0f, 0.0f, 0.0f });

		light = egfx::node::create_directional_light(main_scene.get_root_node());
		light.set_position({ 10.0f, 10.0f, -10.0f });
		light.set_direction(normalized(math::vector3f{ -1.0f, -1.0f, 1.0f }));

		debug_surface = egfx::overlay::create_surface("DebugSurface");
		debug_text.emplace(debug_surface, "DebugText");
		debug_text->set_font("DebugFont");
		debug_text->set_height(0.025);

		debug_surface.show();

		selection_render = egfx::node::create_manual(main_scene.get_root_node());
	}

	brush application::make_brush(std::span<math::plane const> planes, std::string const& name, math::point3f position)
	{
		auto mesh = std::make_shared<egfx::mesh_definition const>(egfx::mesh_definition::make_from_planes(planes));
		auto node = egfx::node::create_mesh(main_scene.get_root_node(), name, *mesh);
		node.set_position(position);
		return { std::move(mesh), std::move(node) };
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

		std::string s;
		selection_context->get_debug_string(s);
		debug_text->set_text(s);

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
