#pragma once

#include "egfx/overlay/text.h"
#include "egfx/overlay/surface.h"
#include "egfx/overlay/panel.h"

#include <string>

namespace ot::dedit::hud
{
	class shadowed_text
	{
		egfx::overlay::panel panel;
		egfx::overlay::text text;
		egfx::overlay::text shadow;

	public:
		shadowed_text(egfx::overlay::surface& overlay, std::string const& text_name);

		shadowed_text& set_font(std::string const& font);
		shadowed_text& set_height(double relative_height);
		shadowed_text& set_text(std::string const& caption);
	};
}