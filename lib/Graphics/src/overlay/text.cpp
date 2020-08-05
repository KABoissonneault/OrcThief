#include "overlay/text.h"

namespace ot::graphics::overlay
{
	void init_text(text& e, Ogre::v1::TextAreaOverlayElement* ptr) noexcept
	{
		init_element(e, ptr);
	}

	Ogre::v1::TextAreaOverlayElement& get_text(text& e) noexcept
	{
		return static_cast<Ogre::v1::TextAreaOverlayElement&>(get_element(e));
	}

	Ogre::v1::TextAreaOverlayElement const& get_text(text const& e) noexcept
	{
		return static_cast<Ogre::v1::TextAreaOverlayElement const&>(get_element(e));
	}

	void text::set_color(color const& c)
	{
		Ogre::ColourValue const ogre_color{ static_cast<float>(c.r), static_cast<float>(c.g), static_cast<float>(c.b), static_cast<float>(c.a) };
		get_text(*this).setColour(ogre_color);
	}

	void text::set_font(std::string const& font)
	{
		get_text(*this).setFontName(font);
	}

	void text::set_height(double height)
	{
		get_text(*this).setHeight(static_cast<Ogre::Real>(height));
	}

	void text::set_text(std::string const& text)
	{
		get_text(*this).setCaption(text);
	}

	text create_text(std::string const& s)
	{
		text t;
		init_text(t, ogre::overlay::make_text_area_element(s));
		return t;
	}
}
