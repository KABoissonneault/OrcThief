#pragma once

namespace ImGui
{
	void PushItemDisabled();
	void PopItemDisabled();
	// Like main menu bar, but at the bottom
	bool BeginMainStatusBar();
	void EndMainStatusBar();
}
