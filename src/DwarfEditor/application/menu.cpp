#include "application/menu.h"

#include "application/application.h"
#include "application/action_handler.h"

#include "input.h"
#include "console.h"

#include "menu/console_window.h"
#include "menu/about_window.h"

#include "action/brush.h"

#include <imgui.h>
#include <fstream>
#include <IconsFontAwesome5.h>

namespace ot::dedit
{
	template<typename Application>
	menu<Application>::menu()
	{
		about_window::load_content();
	}

	template<typename Application>
	bool menu<Application>::handle_keyboard_event(SDL_KeyboardEvent const& e)
	{
		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_action_handler();
		auto& map_handler = app.get_map_handler();
		basic_mesh_repo const& mesh_repo = app.get_mesh_repo();

		if (is_key_press(e, SDLK_n, input::keyboard::mod_group::ctrl))
		{
			map_handler.new_map();
			return true;
		}

		if (is_key_press(e, SDLK_o, input::keyboard::mod_group::ctrl))
		{
			map_handler.open_map();
			return true;
		}

		if (is_key_press(e, SDLK_s, input::keyboard::mod_group::ctrl))
		{
			map_handler.save_map();
			return true;
		}

		if (is_key_press(e, SDLK_s, input::keyboard::mod_combo::ctrl_shift))
		{
			map_handler.save_map_as();
			return true;
		}

		if (is_key_press(e, SDLK_o, input::keyboard::mod_group::alt))
		{
			draw_console_window = !draw_console_window;
			return true;
		}

		if (is_key_press(e, SDLK_b, input::keyboard::mod_combo::ctrl_alt))
		{
			acc.emplace_action<action::spawn_brush>(mesh_repo.get_cube());
			return true;
		}

		return false;
	}

	template<typename Application>
	void menu<Application>::update()
	{
		draw_main_menu();
		draw_main_status();

		if (!draw_console_window)
		{
			size_t const error_count = std::ranges::count(console::get_logs(), console::level_type::error, &console::log_data::level);
			if (error_count > last_error_count)
				draw_console_window = true;
			last_error_count = error_count;
		}

		if (draw_console_window)
		{
			console_window::draw(&draw_console_window);
		}

		if (draw_about_window)
		{
			about_window::draw(&draw_about_window);
		}

		if(draw_imgui_demo)
		{
			ImGui::ShowDemoWindow(&draw_imgui_demo);
		}
	}

	template<typename Application>
	void menu<Application>::draw_main_menu()
	{
		if (!ImGui::BeginMainMenuBar())
			return;

		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_action_handler();
		auto& map_handler = app.get_map_handler();
		map& m = app.get_current_map();
		basic_mesh_repo const& mesh_repo = app.get_mesh_repo();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem(ICON_FA_FILE " New", "CTRL+N"))
			{
				map_handler.new_map();
			}

			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open", "CTRL+O"))
			{
				map_handler.open_map();
			}

			if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Save", "CTRL+S"))
			{
				map_handler.save_map();
			}

			if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Save as...", "CTRL+Shift+S"))
			{
				map_handler.save_map_as();
			}

			ImGui::Separator();

			if (ImGui::MenuItem(ICON_FA_TIMES " Quit", "Alt+F4"))
			{
				app.quit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem(ICON_FA_UNDO_ALT " Undo", "CTRL+Z", false, acc.has_undo()))
			{
				acc.undo_latest(m);
			}

			if (ImGui::MenuItem(ICON_FA_REDO_ALT " Redo", "CTRL+Y", false, acc.has_redo()))
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
					acc.emplace_action<action::spawn_brush>(mesh_repo.get_cube());
				}

				if (ImGui::MenuItem("Octagonal Prism"))
				{
					acc.emplace_action<action::spawn_brush>(mesh_repo.get_octagonal_prism());
				}

				if (ImGui::MenuItem("Hex Prism"))
				{
					acc.emplace_action<action::spawn_brush>(mesh_repo.get_hex_prism());
				}

				if (ImGui::MenuItem("Tri Prism"))
				{
					acc.emplace_action<action::spawn_brush>(mesh_repo.get_tri_prism());
				}

				if (ImGui::MenuItem("Square Pyramid"))
				{
					acc.emplace_action<action::spawn_brush>(mesh_repo.get_square_pyramid());
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", "", &draw_about_window, about_window::has_content());
			ImGui::MenuItem("ImGui Demo", "", &draw_imgui_demo, /*enabled*/ true);

			ImGui::EndMenu();
		}		

		ImGui::EndMainMenuBar();
	}

	template<typename Application>
	void menu<Application>::draw_main_status()
	{
		if (!ImGui::BeginMainStatusBar())
			return;

		derived& app = static_cast<derived&>(*this);
		auto& map_handler = app.get_map_handler();
		
		if (map_handler.has_map_file())
		{
			std::string_view const map_file = map_handler.get_map_file();
			ImGui::Text("%.*s%s", static_cast<int>(map_file.size()), map_file.data(), map_handler.is_map_dirty() ? " (*)" : "");
		}
		else
		{
			ImGui::Text("Unsaved map%s", map_handler.is_map_dirty() ? " (*)" : "");
		}

		std::span<console::log_data const> const logs = console::get_logs();
		if (!logs.empty())
		{
			console::log_data const& last_log = logs.back();

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::SameLine();

			std::optional<ImVec4> color;

			if (color)
				ImGui::PushStyleColor(ImGuiCol_Text, *color);

			ImGui::TextUnformatted(last_log.message.c_str());

			if (ImGui::IsItemClicked())
				draw_console_window = true;

			if (color)
				ImGui::PopStyleColor();
		}

		ImGui::EndMainStatusBar();
	}

	template class menu<application>;
}
