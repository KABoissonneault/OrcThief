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

	// Copy of BeginMenuBar except it's at the bottom
	bool BeginMainStatusBar()
	{
		ImGuiContext& g = *GImGui;
		ImGuiViewport* viewport = g.Viewports[0];
		float height = g.NextWindowData.MenuBarOffsetMinVal.y + g.FontSize + g.Style.FramePadding.y * 2;
		g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
		SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - height));
		SetNextWindowSize(ImVec2(viewport->Size.x, height));
		SetNextWindowViewport(viewport->ID); // Enforce viewport so we don't create our onw viewport when ImGuiConfigFlags_ViewportsNoMerge is set.
		PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		bool is_open = Begin("##MainStatusBar", NULL, window_flags) && BeginMenuBar();
		PopStyleVar(2);
		g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
		if (!is_open)
		{
			End();
			return false;
		}
		return true;
	}

	void EndMainStatusBar()
	{
		EndMenuBar();

		// When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
		// FIXME: With this strategy we won't be able to restore a NULL focus.
		ImGuiContext& g = *GImGui;
		if (g.CurrentWindow == g.NavWindow && g.NavLayer == ImGuiNavLayer_Main && !g.NavAnyRequest)
			FocusTopMostWindowUnderOne(g.NavWindow, NULL);

		End();
	}
}