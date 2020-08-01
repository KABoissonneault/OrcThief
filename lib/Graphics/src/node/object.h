#pragma once

#include "graphics/node/object.h"

namespace ot::graphics::node
{
	void init_object(object& n, Ogre::SceneNode* snode);
	Ogre::SceneNode& get_scene_node(object&);
}