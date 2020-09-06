#include "hud/shadowed_text.h"

namespace ot::hud
{
	shadowed_text::shadowed_text(egfx::overlay::surface& overlay, std::string const& text_name)
	{
		panel = egfx::overlay::create_panel(text_name);
		text = egfx::overlay::create_text(text_name + "Text");
		text.set_color(egfx::color::white());
		shadow = egfx::overlay::create_text(text_name + "Shadow");
		shadow.set_color(egfx::color::black());
		shadow.set_position(0.002, 0.002);

		panel.add_child(text);
		panel.add_child(shadow);
		overlay.add(panel);
	}

	shadowed_text& shadowed_text::set_font(std::string const& font_name)
	{
		text.set_font(font_name);
		shadow.set_font(font_name);
		return *this;
	}

	shadowed_text& shadowed_text::set_height(double relative_height)
	{
		text.set_height(relative_height);
		shadow.set_height(relative_height);
		return *this;
	}

	shadowed_text& shadowed_text::set_text(std::string const& s)
	{
		text.set_text(s);
		shadow.set_text(s);
		return *this;
	}
}