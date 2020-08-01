#pragma once

#include "graphics/node/light.h"
#include "object.h"

#include "Ogre/SceneNode.h"

namespace ot::graphics::node
{
	directional_light make_directional_light(Ogre::SceneNode* node);
}
