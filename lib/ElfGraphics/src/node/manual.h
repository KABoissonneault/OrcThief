#pragma once

#include "egfx/node/manual.h"
#include "node/object.h"

#include "Ogre/ManualObject2.h"

namespace ot::egfx::node
{
	Ogre::ManualObject& get_manual(manual& m);
	Ogre::ManualObject const& get_manual(manual const& m);
}
