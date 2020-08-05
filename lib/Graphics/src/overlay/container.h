#pragma once

#include "graphics/overlay/container.h"
#include "overlay/element.h"
#include "Ogre/Components/Overlay/Container.h"

namespace ot::graphics::overlay
{
	void init_container(container& e, Ogre::v1::OverlayContainer* ptr) noexcept;
	Ogre::v1::OverlayContainer& get_container(container& e) noexcept;
	Ogre::v1::OverlayContainer const& get_container(container const& e) noexcept;
}
