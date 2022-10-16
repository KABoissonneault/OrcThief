#include "debug_menu.h"

#include "application/application.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ot::wf
{
	namespace
	{
		bool imgui_demo_open = false;
		bool enemy_editor_open = false;
	}

	void draw_debug_menu()
	{
		if (ImGui::Begin("DebugMenu"))
		{
			if (ImGui::Button("ImGui Demo"))
			{
				imgui_demo_open = !imgui_demo_open;
			}

			if (ImGui::Button("Enemy Editor"))
			{
				enemy_editor_open = !enemy_editor_open;
			}
		}
		ImGui::End();

		if(imgui_demo_open)
			ImGui::ShowDemoWindow(&imgui_demo_open);

		if (enemy_editor_open)
		{
			application& app = application::get_instance();
			if (ImGui::Begin("EnemyEditor", &enemy_editor_open))
			{
				if (ImGui::Button("Load"))
				{
					app.load_game_data();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					app.save_game_data();
				}

				std::span<m3::enemy_template> enemy_templates = app.get_enemy_templates();
				static int selected_template = -1;

				if (selected_template >= enemy_templates.size())
					selected_template = -1;

				char const* const preview = (selected_template == -1) ? "" :
					enemy_templates[selected_template].name.c_str();

				if (ImGui::BeginCombo("##EnemyTemplate", preview))
				{
					for (int i = 0; i < enemy_templates.size(); ++i)
					{
						bool const selected = selected_template == i;

						std::string alternate_name;
						if (enemy_templates[i].name.empty())
							alternate_name = std::format("<{}>", i);

						char const* const label = enemy_templates[i].name.empty() ? alternate_name.c_str() : enemy_templates[i].name.c_str();
						if (ImGui::Selectable(label, selected))
							selected_template = i;

						if (selected)
							ImGui::SetItemDefaultFocus();
					}

					if (ImGui::Selectable("<New Template>", false))
					{
						m3::enemy_template default_template;
						default_template.attributes.cognition = 50;
						default_template.attributes.cleverness = 50;
						default_template.attributes.focus = 50;
						default_template.attributes.charisma = 50;
						default_template.attributes.will = 50;
						default_template.attributes.wisdom = 50;
						default_template.attributes.strength = 50;
						default_template.attributes.constitution = 50;
						default_template.attributes.agility = 50;
						app.add_enemy_template(default_template);

						enemy_templates = app.get_enemy_templates();
						selected_template = enemy_templates.size() - 1;
					}

					ImGui::EndCombo();
				}

				if (selected_template != -1)
				{
					ImGui::Text("Name"); ImGui::SameLine(); ImGui::InputText("##NameInput", &enemy_templates[selected_template].name);

					m3::character_attributes& att = enemy_templates[selected_template].attributes;
					ImGui::Text("Cognition"); ImGui::SameLine(); ImGui::InputInt("##CognitionInput", &att.cognition);
					ImGui::Text("Cleverness"); ImGui::SameLine(); ImGui::InputInt("##ClevernessInput", &att.cleverness);
					ImGui::Text("Focus"); ImGui::SameLine(); ImGui::InputInt("##FocusInput", &att.focus);
					ImGui::Text("Charisma"); ImGui::SameLine(); ImGui::InputInt("##CharismaInput", &att.charisma);
					ImGui::Text("Will"); ImGui::SameLine(); ImGui::InputInt("##WillInput", &att.will);
					ImGui::Text("Wisdom"); ImGui::SameLine(); ImGui::InputInt("##WisdomInput", &att.wisdom);
					ImGui::Text("Strength"); ImGui::SameLine(); ImGui::InputInt("##StrengthInput", &att.strength);
					ImGui::Text("Constitution"); ImGui::SameLine(); ImGui::InputInt("##ConstitutionInput", &att.constitution);
					ImGui::Text("Agility"); ImGui::SameLine(); ImGui::InputInt("##AgilityInput", &att.agility);
				}
			}
			ImGui::End();
		}
	}
}