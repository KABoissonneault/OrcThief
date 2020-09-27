#include "menu/console_window.h"

#include "console.h"

#include <imgui.h>
#include <optional>

namespace ot::dedit::console_window
{
	const ImVec2 k_default_size(520, 600);
	const ImVec2 k_item_spacing(4, 1);

	void draw(bool* enabled)
	{
		ImGui::SetNextWindowSize(k_default_size, ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Console", enabled, ImGuiWindowFlags_NoFocusOnAppearing))
		{
			ImGui::End();
			return;
		}

		{
			ImGui::BeginChild("ScrollArea##Console", {}, false, ImGuiWindowFlags_HorizontalScrollbar);
			
			bool copy_log = false;
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Copy")) copy_log = true;
				if (ImGui::Selectable("Clear")) console::clear();
				ImGui::EndPopup();
			}
			
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, k_item_spacing);

			if (copy_log)
				ImGui::LogToClipboard();

			auto const logs = console::get_logs();
			for (console::log_data const& log : logs)
			{
				std::optional<ImVec4> color;
				std::string prefix;
				switch (log.level)
				{
				case console::level_type::warning: 
					prefix = "[warning]";
					color = ImVec4(1.f, 1.f, 0.4f, 1.f); 
					break;
				case console::level_type::error: 
					prefix = "[error]";
					color = ImVec4(1.f, 0.4f, 0.4f, 1.f); 
					break;
				}

				if (color)
					ImGui::PushStyleColor(ImGuiCol_Text, *color);

				if (prefix.empty())
					ImGui::TextUnformatted(log.message.c_str());
				else
					ImGui::Text("%s %s", prefix.c_str(), log.message.c_str());

				if (color)
					ImGui::PopStyleColor();
			}

			if (copy_log)
				ImGui::LogFinish();

			ImGui::PopStyleVar();
			ImGui::EndChild();
		}

		ImGui::End();
	}
}
