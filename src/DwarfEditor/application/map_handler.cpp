#include "map_handler.h"

#include "application.h"
#include "console.h"
#include "platform/file_dialog.h"
#include "serialize/serialize_map.h"
#include "input.h"

#include <fmt/format.h>
#include <imgui.h>

namespace ot::dedit
{
	template<typename Application>
	void map_handler<Application>::update()
	{
		switch (current_state)
		{
		case state::confirming_for_new_map:
		case state::confirming_for_open_map:
		case state::confirming_for_quit:
			draw_confirmation_dialog();
			break;
		}
	}

	template<typename Application>
	void map_handler<Application>::draw_confirmation_dialog()
	{
		derived& app = static_cast<derived&>(*this);

		char const* const popup = "Save##SaveConfirmation";
		if (!ImGui::IsPopupOpen(popup))
			ImGui::OpenPopup(popup);

		if (!ImGui::BeginPopupModal(popup, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
			return;

		ImGui::Text("Save changes to the current map?");

		if (ImGui::Button("Yes"))
		{
			if (current_state == state::confirming_for_new_map)
				current_state = state::saving_for_new_map;
			else if (current_state == state::confirming_for_open_map)
				current_state = state::saving_for_open_map;
			else if (current_state == state::confirming_for_quit)
				current_state = state::saving_for_quit;

			save_map();
			
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			if (current_state == state::confirming_for_new_map)
				do_new_map();
			else if (current_state == state::confirming_for_open_map)
				do_open_map();

			current_state = state::none;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel") || input::keyboard::is_pressed(SDL_SCANCODE_ESCAPE))
		{
			app.cancel_quit();
			current_state = state::none;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	template<typename Application>
	void map_handler<Application>::new_map()
	{
		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_action_handler();

		// If map is dirty, confirm before changing
		if (acc.get_last_action() != saved_action)
		{
			current_state = state::confirming_for_new_map;
		}
		else
		{
			do_new_map();
		}
	}

	template<typename Application>
	void map_handler<Application>::do_new_map()
	{
		derived& app = static_cast<derived&>(*this);
		map& m = app.get_current_map();
		action_handler& acc = app.get_action_handler();

		m.clear();
		map_path.clear();
		acc.clear();
		saved_action = 0;

		console::log("Starting new map");
	}

	template<typename Application>
	void map_handler<Application>::open_map()
	{		
		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_action_handler();

		// If map is dirty, confirm before changing
		if (acc.get_last_action() != saved_action)
		{
			current_state = state::confirming_for_open_map;
		}
		else
		{
			do_open_map();
		}
	}

	template<typename Application>
	void map_handler<Application>::do_open_map()
	{
		platform::open_file_dialog(".dem", [this](std::error_code ec, std::string file_path)
		{
			derived& app = static_cast<derived&>(*this);
			map& m = app.get_current_map();
			action_handler& acc = app.get_action_handler();

			if (ec)
			{
				if (ec != std::errc::operation_canceled)
					console::error(fmt::format("Open file failed ({})", ec.message()));
				return;
			}

			m.clear();
			app.map_path.clear();
			acc.clear();
			saved_action = 0;

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
	void map_handler<Application>::save_map()
	{
		derived& app = static_cast<derived&>(*this);

		if (app.map_path.empty())
		{
			save_map_as();
		} 
		else
		{
			map& m = app.get_current_map();
			action_handler& acc = app.get_action_handler();

			// Map unchanged, disregard this
			if (acc.get_last_action() == saved_action)
				return;

			std::FILE* file = std::fopen(app.map_path.c_str(), "w");
			if (file == nullptr)
			{
				console::error(fmt::format("Could not open '{}' for writing", app.map_path));
				return;
			}

			if (!serialize::fwrite(m, file))
			{
				console::error(fmt::format("Failed to save map '{}'", app.map_path));
				std::fclose(file);
			} 
			else
			{
				console::log(fmt::format("Saved map '{}'", app.map_path));
				saved_action = acc.get_last_action();
				std::fclose(file);
				do_post_save_operation();
			}
		}
	}

	template<typename Application>
	void map_handler<Application>::save_map_as()
	{
		platform::file_type file_type;
		file_type.name = "DwarfEditor Map";
		std::string_view const extension = ".dem";
		file_type.extensions = std::span<std::string_view const>(&extension, 1);
		platform::save_file_dialog(file_type, [this](std::error_code ec, std::string file_path)
		{
			derived& app = static_cast<derived&>(*this);
			map& m = app.get_current_map();
			action_handler& acc = app.get_action_handler();

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
				std::fclose(file);
			} 
			else
			{
				app.map_path = std::move(file_path);
				console::log(fmt::format("Saved map as '{}'", app.map_path));
				saved_action = acc.get_last_action();
				std::fclose(file);
				do_post_save_operation();
			}			
		});
	}

	template<typename Application>
	void map_handler<Application>::do_post_save_operation()
	{
		if (current_state == state::saving_for_new_map)
			do_new_map();
		else if (current_state == state::saving_for_open_map)
			do_open_map();
		current_state = state::none;
	}

	template<typename Application>
	void map_handler<Application>::quit()
	{
		derived& app = static_cast<derived&>(*this);
		action_handler& acc = app.get_action_handler();

		if (acc.get_last_action() != saved_action)
		{
			current_state = state::confirming_for_quit;
		}
	}

	template class map_handler<application>;
}
