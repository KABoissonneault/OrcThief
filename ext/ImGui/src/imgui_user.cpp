#include "imgui.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

namespace ImGui
{
	void PushItemDisabled()
	{
		ImGuiContext& g = *GImGui;
		ImGuiStyle& style = g.Style;

		PushItemFlag(ImGuiItemFlags_Disabled, true);
		PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_Text] * ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
		PushStyleColor(ImGuiCol_FrameBg, style.Colors[ImGuiCol_FrameBg] * ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
		PushStyleColor(ImGuiCol_CheckMark, style.Colors[ImGuiCol_CheckMark] * ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
		PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_Button] * ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
		PushStyleColor(ImGuiCol_SliderGrab, style.Colors[ImGuiCol_SliderGrab] * ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
	}

	void PopItemDisabled()
	{
		PopStyleColor(5);
		PopItemFlag();
	}
}