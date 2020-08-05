#pragma once

#include "graphics/overlay/element.h"
#include "graphics/color.h"

#include <string>

namespace ot::graphics::overlay
{
	class text : public element 
	{
	public:
		void set_color(color const& c);
		void set_font(std::string const& font);
		// Set height in normalized [0, 1] coordinates relative to screen height
		void set_height(double height);
		void set_text(std::string const& text);
	};

	[[nodiscard]] text create_text(std::string const& s);
}
