#pragma once

#include "egfx/overlay/element.h"

#include "Ogre/Components/Overlay/Element.h"

namespace ot::egfx::overlay
{
	void init_element(element& e, Ogre::v1::OverlayElement* p) noexcept;
	Ogre::v1::OverlayElement& get_element(element& e) noexcept;
	Ogre::v1::OverlayElement const& get_element(element const& e) noexcept;
}