#include "application/application.h"
#include "config.h"
#include "main_imgui.h"
#include "debug/debug_menu.h"

#include "egfx/window_type.h"
#include "egfx/module.h"
#include "egfx/object/camera.h"

#include <SDL_events.h>
#include <imgui_impl_sdl.h>
#include <im3d.h>

#include <filesystem>
#include <fstream>

namespace ot::wf
{
	namespace
	{
		application* instance;

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

		Im3d::Vec3 get_cursor_ray(egfx::object::camera_cref const camera, math::transform_matrix const& camera_projection)
		{
			Im3d::AppData& ad = Im3d::GetAppData();

			int cursor_x, cursor_y;
			SDL_GetMouseState(&cursor_x, &cursor_y);

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

		std::filesystem::path get_game_data_path(config const& program_config)
		{
			return std::filesystem::path(program_config.get_core().get_resource_root()) / "Game";
		}

		char const* const enemy_template_filename = "enemy_templates.bin";
	}

	application::application(SDL_Window& window, egfx::module& gfx_module, config const& program_config)
		: window(&window)
		, gfx_module(&gfx_module)
		, program_config(&program_config)
		, main_scene(gfx_module, program_config)
	{

	}

	application& application::create_instance(SDL_Window& window, egfx::module& gfx_module, config const& program_config)
	{
		instance = new application(window, gfx_module, program_config);
		return *instance;
	}

	void application::destroy_instance() noexcept
	{
		delete instance;
		instance = nullptr;
	}

	application& application::get_instance()
	{
		return *instance;
	}

	void application::run()
	{
		std::chrono::time_point current_frame = std::chrono::steady_clock::now();
		math::seconds time_buffer = fixed_step; // Start with one step

		bool wants_quit = false;
		bool draw_debug = false;
		while (!wants_quit)
		{
			// Events
			{
				std::vector<egfx::window_event> window_events;

				SDL_Event e;
				while (SDL_PollEvent(&e))
				{
					switch (e.type)
					{
					case SDL_KEYUP:
					case SDL_KEYDOWN:
						ImGui_ImplSDL2_ProcessEvent(&e);

						// Handle alt+f4 pressed on a window other than the main one
						if (ImGui::GetIO().WantCaptureKeyboard)
						{
							if (e.key.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_F4 && (e.key.keysym.mod & KMOD_ALT) == KMOD_ALT)
								wants_quit = true;

							break;
						}

						if (e.key.type == SDL_KEYUP && e.key.keysym.scancode == SDL_SCANCODE_F1)
						{
							draw_debug = !draw_debug;
						}
						break;

					case SDL_WINDOWEVENT:
						ImGui_ImplSDL2_ProcessEvent(&e);
						push_window_event(e, window_events);

						if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(window))
						{
							wants_quit = true;
						}

						break;

					case SDL_TEXTINPUT:
						ImGui_ImplSDL2_ProcessEvent(&e);
						break;

					case SDL_MOUSEBUTTONUP:
					case SDL_MOUSEBUTTONDOWN:
						ImGui_ImplSDL2_ProcessEvent(&e);

						if (ImGui::GetIO().WantCaptureMouse)
							break;

						break;

					case SDL_MOUSEWHEEL:
						ImGui_ImplSDL2_ProcessEvent(&e);

						if (ImGui::GetIO().WantCaptureMouse)
							break;

						break;

					case SDL_MOUSEMOTION:
						ImGui_ImplSDL2_ProcessEvent(&e);

						if (ImGui::GetIO().WantCaptureKeyboard)
							break;

						break;

					case SDL_QUIT:
						wants_quit = true;
						break;
					}
				}

				gfx_module->on_window_events(window_events);
			}

			// Pre-update
			imgui::pre_update();
			gfx_module->pre_update();
			{
				Im3d::AppData& ad = Im3d::GetAppData();
				egfx::object::camera_cref const camera = main_scene.get_camera();
				math::transform_matrix const camera_projection = camera.get_projection();

				ad.m_cursorRayOrigin = ad.m_viewOrigin;
				ad.m_cursorRayDirection = get_cursor_ray(camera, camera_projection);

				Uint32 const mouse_state = SDL_GetMouseState(nullptr, nullptr);
				ad.m_keyDown[Im3d::Mouse_Left] = (mouse_state & SDL_BUTTON_LMASK) == SDL_BUTTON_LEFT;
			}

			// Fixed Update
			while (time_buffer >= fixed_step)
			{
				main_scene.update(fixed_step);

				time_buffer -= fixed_step;
			}

			// Render
			main_scene.render();

			if (draw_debug)
			{
				draw_debug_menu();
			}

			if (!gfx_module->render())
				wants_quit = true;

			// End frame
			imgui::end_frame();

			std::chrono::time_point const last_frame = std::exchange(current_frame, std::chrono::steady_clock::now());
			time_buffer += current_frame - last_frame;
		}
	}
	
	namespace
	{
		enum class enemy_template_versions : int
		{
			initial = 0
		};

		enemy_template_versions const enemy_template_version = enemy_template_versions::initial;
	}

	void application::save_game_data()
	{
		std::filesystem::path const game_data_path = get_game_data_path(*program_config);
		std::filesystem::create_directory(game_data_path);

		std::ofstream o(game_data_path / enemy_template_filename, std::ios::binary);
		if (!o)
			throw std::runtime_error(std::format("Could not save enemy template, '{}' could not be opened for write in game data folder", enemy_template_filename));

		o.write((char const*)&enemy_template_version, sizeof(enemy_template_version));
		
		int const template_count = (int)enemy_templates.size();
		o.write((char const*)&template_count, sizeof(template_count));
		for (m3::enemy_template const& e : enemy_templates)
		{
			unsigned short const name_size = (unsigned short)e.name.size();
			o.write((char const*)&name_size, sizeof(name_size));
			o.write(e.name.c_str(), (std::streamsize)e.name.size());
			
			o.write((char const*)&e.attributes, sizeof(e.attributes));
		}
	}

	void application::load_game_data()
	{
		enemy_templates.clear();

		std::ifstream i(get_game_data_path(*program_config) / enemy_template_filename, std::ios::binary);
		if (!i)
			return;

		enemy_template_versions version;
		if (!(i.read((char*)&version, sizeof(version)))) throw std::runtime_error("Invalid enemy template format: expected version integer");
		if (version > enemy_template_version) throw std::runtime_error("Invalid enemy template format: unsupported version");

		int template_count;
		if (!(i.read((char*)&template_count, sizeof(template_count)))) throw std::runtime_error("Invalid enemy template format: expected template count");
		if (template_count < 0) throw std::runtime_error("Invalid enemy template format: negative template count");

		enemy_templates.resize(template_count);
		for (m3::enemy_template& e : enemy_templates)
		{
			unsigned short name_size;
			if (!(i.read((char*)&name_size, sizeof(name_size))))
			{
				if (i.eof()) throw std::runtime_error(std::format("Invalid enemy template format: Missing templates, expected {}", template_count));
				else throw std::runtime_error("Invalid enemy template format: expected integer name size");
			}
			if (name_size < 0) throw std::runtime_error("Invalid enemy template format: negative name size");
			
			e.name.resize(name_size);
			if (!i.read(e.name.data(), name_size)) throw std::runtime_error("Invalid enemy template format: could not read template name");

			if (!i.read((char*)&e.attributes, sizeof(e.attributes))) throw std::runtime_error("Invalid enemy template format: could not read enemy attributes");
		}
	}

	std::span<m3::enemy_template> application::get_enemy_templates() noexcept
	{
		return enemy_templates;
	}

	void application::add_enemy_template(m3::enemy_template const& t)
	{
		enemy_templates.push_back(t);
	}
}