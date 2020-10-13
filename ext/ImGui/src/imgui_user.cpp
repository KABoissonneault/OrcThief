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
		PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
	}

	void PopItemDisabled()
	{
		PopStyleVar();
		PopItemFlag();
	}
}