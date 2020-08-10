#pragma once

#include "graphics/node/manual.h"
#include "node/object.h"

#include "Ogre/ManualObject2.h"

namespace ot::graphics::node
{
	Ogre::ManualObject& get_manual(manual& m);
	Ogre::ManualObject const& get_manual(manual const& m);
}
