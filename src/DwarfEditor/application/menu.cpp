#include "application/menu.h"

#include "application/application.h"
#include "application/action_handler.h"

#include "menu/console_window.h"
#include "input.h"

#include <imgui.h>


namespace ot::dedit
{
	template<typename Application>
	bool menu<Application>::handle_keyboard_event(SDL_KeyboardEvent const& e)
	{
		if (e.keysym.sym == SDLK_o
			&& e.state == SDL_PRESSED
			&& e.repeat == 0
			&& input::keyboard::get_modifiers() == input::keyboard::mod_group::alt)
		{
			draw_console_window = !draw_console_window;
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

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "Alt+F4"))
			{
				app.quit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			action_handler& actions = app.get_actions();

			if (ImGui::MenuItem("Undo", "CTRL+Z", false, actions.has_undo()))
			{
				actions.undo_latest(app.get_current_map());
			}

			if (ImGui::MenuItem("Redo", "CTRL+Y", false, actions.has_redo()))
			{
				actions.redo_latest(app.get_current_map());
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Console", "Alt+O", &draw_console_window);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	template class menu<application>;
}
