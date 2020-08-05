#pragma once

#include "graphics/overlay/text.h"
#include "graphics/overlay/surface.h"
#include "graphics/overlay/panel.h"

#include <string>

namespace ot::hud
{
	class shadowed_text
	{
		graphics::overlay::panel panel;
		graphics::overlay::text text;
		graphics::overlay::text shadow;

	public:
		shadowed_text(graphics::overlay::surface& overlay, std::string const& text_name);

		shadowed_text& set_font(std::string const& font);
		shadowed_text& set_height(double relative_height);
		shadowed_text& set_text(std::string const& caption);
	};
}