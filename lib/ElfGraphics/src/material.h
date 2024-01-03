#pragma once

#include "egfx/material.h"

#include <OgreIdString.h>

namespace ot::egfx
{
	Ogre::IdString to_id_string(material_handle_t const& h);
	material_handle_t to_material_handle(Ogre::IdString const& s);
}
