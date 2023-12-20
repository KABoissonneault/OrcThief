#include "imgui.h"

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
		ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)GetMainViewport();

		// Notify of viewport change so GetFrameHeight() can be accurate in case of DPI change
		SetCurrentViewport(NULL, viewport);

		// For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
		// FIXME: This could be generalized as an opt-in way to clamp window->DC.CursorStartPos to avoid SafeArea?
		// FIXME: Consider removing support for safe area down the line... it's messy. Nowadays consoles have support for TV calibration in OS settings.
		g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
				
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		float height = GetFrameHeight();
		SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - height));
		SetNextWindowSize(ImVec2(viewport->Size.x, height));
		bool is_open = BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags);
		g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);

		if (is_open)
			BeginMenuBar();
		else
			End();
		return is_open;
	}

	void EndMainStatusBar()
	{
		EndMenuBar();

		// When the user has left the menu layer (typically: closed menus through activation of an item), we restore focus to the previous window
		// FIXME: With this strategy we won't be able to restore a NULL focus.
		ImGuiContext& g = *GImGui;
		if (g.CurrentWindow == g.NavWindow && g.NavLayer == ImGuiNavLayer_Main && !g.NavAnyRequest)
			FocusTopMostWindowUnderOne(g.NavWindow, NULL, NULL, ImGuiFocusRequestFlags_UnlessBelowModal | ImGuiFocusRequestFlags_RestoreFocusedChild);

		End();
	}
}