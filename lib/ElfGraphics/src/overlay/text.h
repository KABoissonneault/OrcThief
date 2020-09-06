#pragma once

#include "egfx/overlay/text.h"
#include "overlay/element.h"

#include "Ogre/Components/Overlay/TextAreaElement.h"

namespace ot::egfx::overlay
{
	void init_text(text& e, Ogre::v1::TextAreaOverlayElement* ptr) noexcept;
	Ogre::v1::TextAreaOverlayElement& get_text(text& e) noexcept;
	Ogre::v1::TextAreaOverlayElement const& get_text(text const& e) noexcept;
}
