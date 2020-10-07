#include "application/menu.h"

#include "application/application.h"
#include "application/action_handler.h"

#include "input.h"
#include "console.h"
#include "menu/console_window.h"
#include "platform/file_dialog.h"
#include "serialize/serialize_map.h"

#include "action/brush.h"

#include <imgui.h>
#include <fmt/format.h>
#include <fstream>

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
	}

	template<typename Application>
	menu<Application>::menu()
		: cube(std::make_shared<egfx::mesh_definition>(cube_planes))
		, octagonal_prism(std::make_shared<egfx::mesh_definition>(octagon_planes))
		, square_pyramid(std::make_shared<egfx::mesh_definition>(pyramid_planes))
	{

	}

	template<typename Application>
	bool menu<Application>::handle_keyboard_event(SDL_KeyboardEvent const& e)
	{
		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_actions();
		map& m = app.get_current_map();

		if (is_key_press(e, SDLK_n, input::keyboard::mod_group::ctrl))
		{
			new_map();
			return true;
		}

		if (is_key_press(e, SDLK_o, input::keyboard::mod_group::ctrl))
		{
			open_map();
			return true;
		}

		if (is_key_press(e, SDLK_s, input::keyboard::mod_group::ctrl))
		{
			save_map();
			return true;
		}

		if (is_key_press(e, SDLK_s, input::keyboard::mod_combo::ctrl_shift))
		{
			save_map_as();
			return true;
		}

		if (is_key_press(e, SDLK_o, input::keyboard::mod_group::alt))
		{
			draw_console_window = !draw_console_window;
			return true;
		}

		if (is_key_press(e, SDLK_b, input::keyboard::mod_combo::ctrl_alt))
		{
			acc.emplace_action<action::spawn_brush>(cube, m.allocate_entity_id());
			return true;
		}

		return false;
	}

	template<typename Application>
	void menu<Application>::update()
	{
		if (draw_console_window)
		{
			console_window::draw(&draw_console_window);
		}

		if (!ImGui::BeginMainMenuBar())
			return;

		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_actions();
		map& m = app.get_current_map();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "CTRL+N"))
			{
				new_map();
			}

			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				open_map();
			}

			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				save_map();
			}

			if (ImGui::MenuItem("Save As...", "CTRL+Shift+S"))
			{
				save_map_as();
			}

			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				app.quit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, acc.has_undo()))
			{
				acc.undo_latest(m);
			}

			if (ImGui::MenuItem("Redo", "CTRL+Y", false, acc.has_redo()))
			{
				acc.redo_latest(m);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Console", "Alt+O", &draw_console_window);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Brush"))
		{
			if (ImGui::BeginMenu("Spawn Primitive"))
			{
				if (ImGui::MenuItem("Cube", "CTRL+Alt+B"))
				{
					acc.emplace_action<action::spawn_brush>(cube, m.allocate_entity_id());
				}

				if (ImGui::MenuItem("Octagonal Prism"))
				{
					acc.emplace_action<action::spawn_brush>(octagonal_prism, m.allocate_entity_id());
				}

				if (ImGui::MenuItem("Square Pyramid"))
				{
					acc.emplace_action<action::spawn_brush>(square_pyramid, m.allocate_entity_id());
				}

				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	template<typename Application>
	void menu<Application>::new_map()
	{
		derived& app = static_cast<derived&>(*this);
		map& m = app.get_current_map();

		// TODO: confirmation on dirty map
		m.clear();
		app.map_path.clear();

		console::log("Starting new map");
	}

	template<typename Application>
	void menu<Application>::open_map()
	{
		derived& app = static_cast<derived&>(*this);

		// TODO: confirmation on dirty map
		platform::open_file_dialog(".dem", [&app](std::error_code ec, std::string file_path)
		{
			map& m = app.get_current_map();

			if (ec)
			{
				if (ec != std::errc::operation_canceled)
					console::error(fmt::format("Open file failed ({})", ec.message()));
				return;
			}

			m.clear();
			app.map_path.clear();

			std::FILE* file = std::fopen(file_path.c_str(), "r");
			assert(file != nullptr);
			if (!serialize::fread(m, file))
			{
				m.clear();
				console::error(fmt::format("Failed loading map '{}'", file_path));
			}
			else
			{

				app.map_path = std::move(file_path);
				console::log(fmt::format("Opened map '{}'", app.map_path));
			}
			std::fclose(file);
		});
	}

	template<typename Application>
	void menu<Application>::save_map()
	{
		derived& app = static_cast<derived&>(*this);
		map& m = app.get_current_map();

		if (app.map_path.empty())
		{
			save_map_as();
		}
		else
		{
			std::FILE* file = std::fopen(app.map_path.c_str(), "w");
			if (file == nullptr)
			{
				console::error(fmt::format("Could not open '{}' for writing", app.map_path));
				return;
			}
			
			if (!serialize::fwrite(m, file))
			{
				console::error(fmt::format("Failed to save map '{}'", app.map_path));
			}
			else
			{
				console::log(fmt::format("Saved map '{}'", app.map_path));
			}
			std::fclose(file);
		}
	}

	template<typename Application>
	void menu<Application>::save_map_as()
	{
		derived& app = static_cast<derived&>(*this);

		platform::file_type file_type;
		file_type.name = "DwarfEditor Map";
		std::string_view const extension = ".dem";
		file_type.extensions = std::span<std::string_view const>(&extension, 1);
		platform::save_file_dialog(file_type, [&app](std::error_code ec, std::string file_path)
		{
			map& m = app.get_current_map();

			if (ec)
			{
				if (ec != std::errc::operation_canceled)
					console::error(fmt::format("Save file failed ({})", ec.message()));
				return;
			}

			std::FILE* file = std::fopen(file_path.c_str(), "w");
			if (file == nullptr)
			{
				console::error(fmt::format("Could not open '{}' for writing", file_path));
				return;
			}

			if (!serialize::fwrite(m, file))
			{
				console::error(fmt::format("Failed to save map as '{}'", file_path));
			}
			else
			{
				app.map_path = std::move(file_path);
				console::log(fmt::format("Saved map as '{}'", app.map_path));
			}
			std::fclose(file);			
		});
	}

	template class menu<application>;
}
