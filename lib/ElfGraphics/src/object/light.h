#pragma once

#include "egfx/object/light.h"
#include "object.h"

namespace ot::egfx
{
	light_ref make_light_ref(Ogre::Light&) noexcept;
	light_cref make_light_cref(Ogre::Light const&) noexcept;
	Ogre::Light& get_light(light_ref) noexcept;
	Ogre::Light const& get_light(light_cref) noexcept;
}
