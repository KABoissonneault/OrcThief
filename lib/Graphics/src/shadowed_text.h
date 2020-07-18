#pragma once

#include "Ogre/Prerequisites.h"
#include "Ogre/Components/Overlay/TextAreaElement.h"

namespace ot::graphics
{
	class shadowed_text
	{
		Ogre::v1::TextAreaOverlayElement* text;
		Ogre::v1::TextAreaOverlayElement* shadow;

	public:
		// Creates OverlayContainers and adds them to the overlay
		shadowed_text(Ogre::v1::Overlay* overlay, const ogre::string& text_name);

		shadowed_text& set_font(const ogre::string& font);
		// Sets height as a ratio (between 0 and 1) of the size of the screen
		shadowed_text& set_height(float relative_height);
		shadowed_text& set_text(const Ogre::v1::DisplayString& caption);

	};
}