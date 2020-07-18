#include "shadowed_text.h"

#include "Ogre/Components/Overlay/Manager.h"
#include "Ogre/Components/Overlay.h"
#include "Ogre/Components/Overlay/PanelElement.h"

namespace ot::graphics
{
	shadowed_text::shadowed_text(Ogre::v1::Overlay* overlay, const ogre::string& text_name)
	{
		auto const panel = ot::ogre::overlay::make_panel_element(text_name);
		text = ot::ogre::overlay::make_text_area_element(text_name + "Text");
		shadow = ot::ogre::overlay::make_text_area_element(text_name + "Shadow");
		shadow->setColour(Ogre::ColourValue::Black);
		shadow->setPosition(0.002f, 0.002f);

		panel->addChild(text);
		panel->addChild(shadow);
		overlay->add2D(panel);
	}

	shadowed_text& shadowed_text::set_font(const ogre::string& font_name)
	{
		text->setFontName(font_name);
		shadow->setFontName(font_name);
		return *this;
	}

	shadowed_text& shadowed_text::set_height(float relative_height)
	{
		text->setHeight(relative_height);
		shadow->setHeight(relative_height);
		return *this;
	}

	shadowed_text& shadowed_text::set_text(const Ogre::v1::DisplayString& caption)
	{
		text->setCaption(caption);
		shadow->setCaption(caption);
		return *this;
	}
}
