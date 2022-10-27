#include "application/ui.h"

#include "math/ops.h"
#include "m3/character.h"

#include <SDL_video.h>
#include <imgui.h>

namespace ot::wf::ui
{
	void draw_player_vitals(SDL_Window& window, std::span<m3::player_character_data const> player_data)
	{
		int window_width, window_height;
		SDL_GetWindowSize(&window, &window_width, &window_height);

		size_t const player_count = math::min_value(player_data.size(), 6);

		ImGui::SetNextWindowSize(ImVec2((player_count + 1) * 128.f, 256.f));
		ImGui::SetNextWindowPos(ImVec2(window_width / 2, window_height * 0.75f), ImGuiCond_None, ImVec2(0.5f, 0));
		if (ImGui::Begin("##CharacterHud", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
		{
			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i + 1));
				ImGui::Text("%s", player_data[i].name.c_str());
			}

			ImGui::NewLine();

			ImGui::Text("Energy");

			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i + 1));

				int const energy_percent = player_data[i].vitals.current_energy * 100 / player_data[i].vitals.max_energy;

				if (energy_percent > 80)
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "Full");
				}
				else if (energy_percent > 60)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.2f, 1.0f), "Winded");
				}
				else if (energy_percent > 40)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.2f, 1.0f), "Sweated");
				}
				else if (energy_percent > 20)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Exhausted");
				}
				else if (energy_percent > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.2f, 1.0f), "Critical");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Knocked Out");
				}
			}

			ImGui::Text("Resolve");
			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i + 1));

				int const resolve_percent = player_data[i].vitals.current_resolve * 100 / player_data[i].vitals.max_resolve;
				if (resolve_percent > 80)
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "Confident");
				}
				else if (resolve_percent > 60)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.2f, 1.0f), "Stressed");
				}
				else if (resolve_percent > 40)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.2f, 1.0f), "Nervous");
				}
				else if (resolve_percent > 20)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Panicking");
				}
				else if (resolve_percent > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.2f, 1.0f), "Critical");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Surrendered");
				}
			}

			ImGui::Text("Health");
			for (int i = 0; i < player_count; ++i)
			{
				ImGui::SameLine(128.f * (i + 1));

				int const health_percent = player_data[i].vitals.current_health * 100 / player_data[i].vitals.max_health;
				if (health_percent > 80)
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "Top Shape");
				}
				else if (health_percent > 60)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.2f, 1.0f), "Scratched");
				}
				else if (health_percent > 40)
				{
					ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.2f, 1.0f), "Injured");
				}
				else if (health_percent > 20)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Wounded");
				}
				else if (health_percent > 0)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.2f, 1.0f), "Critical");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Dead");
				}
			}
		}
		ImGui::End();
	}

	void draw_player_sheet(SDL_Window& window, m3::player_character_data const& player)
	{
		int window_width, window_height;
		SDL_GetWindowSize(&window, &window_width, &window_height);

		ImGui::SetNextWindowSize(ImVec2(1024.f, 512.f));
		ImGui::SetNextWindowPos(ImVec2(window_width / 2, window_height * 0.25f), ImGuiCond_None, ImVec2(0.5f, 0));
		if (ImGui::Begin("##CharacterSheet", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
		{
			ImGui::Text("%s", player.name.c_str());

			ImGui::NewLine();
			ImGui::Text("Cleverness: %d", player.attributes.cleverness);
			ImGui::Text("Hardiness: %d", player.attributes.hardiness);
			ImGui::Text("Focus: %d", player.attributes.focus);
			ImGui::Text("Charisma: %d", player.attributes.charisma);
			ImGui::Text("Will: %d", player.attributes.will);
			ImGui::Text("Wisdom: %d", player.attributes.wisdom);
			ImGui::Text("Strength: %d", player.attributes.strength);
			ImGui::Text("Constitution: %d", player.attributes.constitution);
			ImGui::Text("Agility: %d", player.attributes.agility);

			ImGui::NewLine();
			if (player.skills.military > 0)
				ImGui::Text("Military: %d", player.skills.military);
			if (player.skills.hunt > 0)
				ImGui::Text("Hunt: %d", player.skills.hunt);
			if (player.skills.brawl > 0)
				ImGui::Text("Brawl: %d", player.skills.brawl);
			if (player.skills.dueling > 0)
				ImGui::Text("Dueling: %d", player.skills.dueling);

			if (player.skills.foresting > 0)
				ImGui::Text("Foresting: %d", player.skills.foresting);
			if (player.skills.mountaineering > 0)
				ImGui::Text("Mountaineering: %d", player.skills.mountaineering);
			if (player.skills.sailoring > 0)
				ImGui::Text("Sailoring: %d", player.skills.sailoring);
			if (player.skills.urbanism > 0)
				ImGui::Text("Urbanism: %d", player.skills.urbanism);

			if (player.skills.rhetoric > 0)
				ImGui::Text("Rhetoric: %d", player.skills.rhetoric);
			if (player.skills.astrology > 0)
				ImGui::Text("Astrology: %d", player.skills.astrology);
			if (player.skills.medecine > 0)
				ImGui::Text("Medicine: %d", player.skills.medecine);
			if (player.skills.alchemy > 0)
				ImGui::Text("Alchemy: %d", player.skills.alchemy);
		}
		ImGui::End();
	}
}
