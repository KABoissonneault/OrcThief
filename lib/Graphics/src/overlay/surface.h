#pragma once

#include "graphics/overlay/surface.h"

#include "Ogre/Components/Overlay.h"

namespace ot::graphics::overlay
{
	void init_surface(surface& s, Ogre::v1::Overlay* p) noexcept;
	Ogre::v1::Overlay& get_surface(surface& s) noexcept;
	Ogre::v1::Overlay const& get_surface(surface const& s) noexcept;
}
