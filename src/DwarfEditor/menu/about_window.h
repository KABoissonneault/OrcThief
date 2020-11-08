#pragma once

namespace ot::dedit::about_window
{
	// To be called once before drawing the window
	void load_content();
	void unload_content();

	// Fast query to test whether the About window has content
	bool has_content();

	void draw(bool* open);
}
