#pragma once

namespace ot::egfx
{
	class window;

	int get_width(window const& w) noexcept;
	int get_height(window const& w) noexcept;
	bool has_focus(window const& w) noexcept;
}
