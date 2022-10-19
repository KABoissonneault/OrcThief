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
		bool player_editor_open = false;

		void edit_attributes(m3::character_attributes& att)
		{
			ImGui::Text("Cleverness"); ImGui::SameLine(); ImGui::InputInt("##ClevernessInput", &att.cleverness);
			if (att.cleverness < 0)
				att.cleverness = 0;

			ImGui::Text("Hardiness"); ImGui::SameLine(); ImGui::InputInt("##HardinessInput", &att.hardiness);
			if (att.hardiness < 0)
				att.hardiness = 0;

			ImGui::Text("Focus"); ImGui::SameLine(); ImGui::InputInt("##FocusInput", &att.focus);
			if (att.focus < 0)
				att.focus = 0;

			ImGui::Text("Charisma"); ImGui::SameLine(); ImGui::InputInt("##CharismaInput", &att.charisma);
			if (att.charisma < 0)
				att.charisma = 0;

			ImGui::Text("Will"); ImGui::SameLine(); ImGui::InputInt("##WillInput", &att.will);
			if (att.will < 0)
				att.will = 0;

			ImGui::Text("Wisdom"); ImGui::SameLine(); ImGui::InputInt("##WisdomInput", &att.wisdom);
			if (att.wisdom < 0)
				att.wisdom = 0;

			ImGui::Text("Strength"); ImGui::SameLine(); ImGui::InputInt("##StrengthInput", &att.strength);
			if (att.strength < 0)
				att.strength = 0;

			ImGui::Text("Constitution"); ImGui::SameLine(); ImGui::InputInt("##ConstitutionInput", &att.constitution);
			if (att.constitution < 0)
				att.constitution = 0;

			ImGui::Text("Agility"); ImGui::SameLine(); ImGui::InputInt("##AgilityInput", &att.agility);
			if (att.agility < 0)
				att.agility = 0;
		}

		void edit_vitals(m3::character_vitals& vit)
		{
			ImGui::Text("Max Energy"); ImGui::SameLine(); ImGui::InputInt("##MaxEnergyInput", &vit.max_energy);
			if (vit.max_energy < 1)
				vit.max_energy = 1;

			ImGui::Text("Current Energy"); ImGui::SameLine(); ImGui::InputInt("##CurrentEnergyInput", &vit.current_energy);
			if (vit.current_energy < 0)
				vit.current_energy = 0;
			if (vit.current_energy > vit.max_energy)
				vit.current_energy = vit.max_energy;
			
			ImGui::Text("Max Resolve"); ImGui::SameLine(); ImGui::InputInt("##MaxResolveInput", &vit.max_resolve);
			if (vit.max_resolve < 1)
				vit.max_resolve = 1;

			ImGui::Text("Current Resolve"); ImGui::SameLine(); ImGui::InputInt("##CurrentResolveInput", &vit.current_resolve);
			if (vit.current_resolve < 0)
				vit.current_resolve = 0;
			if (vit.current_resolve > vit.max_resolve)
				vit.current_resolve = vit.max_resolve;

			ImGui::Text("Max Health"); ImGui::SameLine(); ImGui::InputInt("##MaxHealthInput", &vit.max_health);
			if (vit.max_health < 1)
				vit.max_health = 1;

			ImGui::Text("Current Health"); ImGui::SameLine(); ImGui::InputInt("##CurrentHealthInput", &vit.current_health);
			if (vit.current_health < 0)
				vit.current_health = 0;
			if (vit.current_health > vit.max_health)
				vit.current_health = vit.max_health;
		}

		void draw_enemy_editor()
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
						default_template.attributes.cleverness = 50;
						default_template.attributes.hardiness = 50;
						default_template.attributes.focus = 50;
						default_template.attributes.charisma = 50;
						default_template.attributes.will = 50;
						default_template.attributes.wisdom = 50;
						default_template.attributes.strength = 50;
						default_template.attributes.constitution = 50;
						default_template.attributes.agility = 50;
						app.add_enemy_template(default_template);

						enemy_templates = app.get_enemy_templates();
						selected_template = (int)enemy_templates.size() - 1;
					}

					ImGui::EndCombo();
				}

				if (selected_template != -1)
				{
					ImGui::Text("Name"); ImGui::SameLine(); ImGui::InputText("##NameInput", &enemy_templates[selected_template].name);

					edit_attributes(enemy_templates[selected_template].attributes);
				}
			}
			ImGui::End();
		}

		void draw_player_editor()
		{			
			if (ImGui::Begin("PlayerEditor", &player_editor_open))
			{
				application& app = application::get_instance();

				std::span<m3::player_character_data> player_data = app.get_player_data();
				static int selected_template = 0;

				if (player_data.size() == 0)
					return;

				if (selected_template >= player_data.size())
					selected_template = 0;

				char const* const preview = player_data[selected_template].name.c_str();

				if (ImGui::BeginCombo("##PlayerSelection", preview))
				{
					for (int i = 0; i < player_data.size(); ++i)
					{
						bool const selected = selected_template == i;

						char const* const label = player_data[i].name.c_str();
						if (ImGui::Selectable(label, selected))
							selected_template = i;

						if (selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (ImGui::Button("Reset Vitals"))
				{
					player_data[selected_template].vitals = m3::generate_initial_vitals(player_data[selected_template].attributes);
				}

				ImGui::Text("Name"); ImGui::SameLine(); ImGui::InputText("##NameInput", &player_data[selected_template].name);

				edit_attributes(player_data[selected_template].attributes);

				ImGui::NewLine();
				m3::character_vitals& vit = player_data[selected_template].vitals;
				edit_vitals(vit);
			}
			ImGui::End();			
		}
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

			if (ImGui::Button("Player Editor"))
			{
				player_editor_open = !player_editor_open;
			}
		}
		ImGui::End();

		if(imgui_demo_open)
			ImGui::ShowDemoWindow(&imgui_demo_open);
		
		if (enemy_editor_open)
		{
			draw_enemy_editor();
		}

		if (player_editor_open)
		{
			draw_player_editor();
		}
	}
}