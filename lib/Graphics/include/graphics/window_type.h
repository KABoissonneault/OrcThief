#pragma once

#include <variant>

namespace ot::graphics
{
	enum class window_id : uint32_t {};

	struct window_event
	{
		struct moved
		{
			int new_x;
			int new_y;
		};

		struct resized
		{
			int new_width;
			int new_height;
		};

		struct focus_gained {};
		struct focus_lost {};

		window_id id;
		std::variant<moved, resized, focus_gained, focus_lost> data;
	};
}