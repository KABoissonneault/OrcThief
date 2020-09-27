#include "application/menu.h"

#include "application/application.h"
#include "application/action_handler.h"

#include <imgui.h>

namespace ot::dedit
{
	template<typename Application>
	void menu<Application>::update()
	{
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
				app.get_actions().redo_latest(app.get_current_map());
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	template class menu<application>;
}
