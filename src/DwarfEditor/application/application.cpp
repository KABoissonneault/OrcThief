#include "application.h"

#include "input.h"
#include "selection/base_context.h"
#include "imgui/module.h"

#include "egfx/scene.h"
#include "egfx/object/camera.h"
#include "egfx/node/light.h"

#include "Ogre/ConfigOptionMap.h"
#include "Ogre/PlatformInformation.h"
#include "Ogre/RenderSystem.h"

#include "SDL2/window.h"
#include "SDL2/macro.h"
#include <SDL_video.h>

#include <imgui_impl_sdl2.h>
#include <im3d.h>

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
		: menu(program_config)
		, main_window(std::move(window))
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
		selection_context.reset(new selection::base_context(mesh_repo, current_map, main_scene, render_window));

		egfx::object::camera_ref const camera = main_scene.get_camera();
		camera.set_position({ 0.0f, 2.0f, 5.5f });
		camera.look_at({ 0.0f, 0.0f, 0.0f });

		light = egfx::node::create_directional_light(main_scene.get_root_node());
		light.set_position({ 10.0f, 10.0f, 10.0f });
		light.set_direction(normalized(math::vector3f{ -1.0f, -1.0f, -1.0f }));
	}

	void application::run()
	{
		using namespace ot::math::literals;
		auto last_frame = std::chrono::steady_clock::now();
		auto current_frame = last_frame;

		while (!wants_quit || !map_handler::can_quit()) 
		{
			start_frame();
		
			handle_events();

			pre_update();

			update(current_frame - last_frame);

			if (!render())
			{
				quit();
			}

			end_frame();

			last_frame = std::exchange(current_frame, std::chrono::steady_clock::now());
		}
	}

	void application::quit()
	{
		wants_quit = true;
		map_handler::quit();
	}

	void application::start_frame()
	{
		mouse.start_frame();
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
				if (camera_controller::handle_keyboard_event(e.key))
					break;

				ImGui_ImplSDL2_ProcessEvent(&e);

				// Handle alt+f4 pressed on a window other than the main one
				if (imgui_io.WantCaptureKeyboard && is_key_press(e.key, SDLK_F4, input::keyboard::mod::lalt))
				{
					quit();
					break;
				}

				// All these contexts are expected to check ImGui::GetIO().WantCaptureKeyboard/WantTextInput
				
				if (selection_actions.handle_keyboard_event(e.key, current_map))
					break;

				if (menu::handle_keyboard_event(e.key))
					break;

				if (selection_context != nullptr && selection_context->handle_keyboard_event(e.key, selection_actions))
					break;

				break;
			}
			case SDL_TEXTINPUT:
				if (camera_controller::handle_text_event(e.text))
					break;

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
				if (camera_controller::handle_mouse_wheel_event(e.wheel))
					break;

				ImGui_ImplSDL2_ProcessEvent(&e);
				
				if (imgui::has_mouse())
					break;

				break;

			case SDL_MOUSEMOTION:
				if (camera_controller::handle_mouse_motion_event(e.motion))
					break;

				if (mouse.handle_mouse_motion_event(e.motion))
					break;

				ImGui_ImplSDL2_ProcessEvent(&e);

				break;

			case SDL_WINDOWEVENT:
				ImGui_ImplSDL2_ProcessEvent(&e);
				push_window_event(e, window_events);

				if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(main_window.get()))
				{
					quit();
				}

				break;

			case SDL_QUIT:
				quit();
				break;
			}
		}

		graphics.on_window_events(window_events);
	}

	void application::pre_update()
	{
		imgui::pre_update();
		graphics.pre_update();
		update_im3d();
	}

	void application::update(math::seconds dt)
	{
		menu::update();

		map_handler::update();

		camera_controller::update(dt);

		input::frame_input input;
		input.mouse_action = mouse.get_action();

		selection_context->update(selection_actions, input);

		selection_actions.apply_actions(current_map);
	}

	namespace
	{
		Im3d::Vec3 get_cursor_ray(egfx::object::camera_cref const camera, math::transform_matrix const& camera_projection)
		{
			Im3d::AppData& ad = Im3d::GetAppData();

			int cursor_x, cursor_y;
			input::mouse::get_position(cursor_x, cursor_y);

			float const norm_x = (static_cast<float>(cursor_x) / ad.m_viewportSize.x) * 2.0f - 1.0f;
			float const norm_y = -((static_cast<float>(cursor_y) / ad.m_viewportSize.y) * 2.0f - 1.0f);

			math::transform_matrix const camera_transform = camera.get_transformation();

			math::vector3f const ray_direction
			{
				norm_x / camera_projection[0][0]
				, norm_y / camera_projection[1][1]
				, -1.0f
			};

			return normalized(transform(ray_direction, camera_transform));
		}
	}

	void application::update_im3d()
	{
		Im3d::AppData& ad = Im3d::GetAppData();
		egfx::object::camera_cref const camera = main_scene.get_camera();
		math::transform_matrix const camera_projection = camera.get_projection();
		
		ad.m_cursorRayOrigin = ad.m_viewOrigin;
		ad.m_cursorRayDirection = get_cursor_ray(camera, camera_projection);
		ad.m_keyDown[Im3d::Mouse_Left] = input::mouse::get_buttons() == input::mouse::button_type::left;
	}

	bool application::render()
	{
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
