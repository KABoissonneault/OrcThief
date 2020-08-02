#pragma once

#include "graphics/node/object.h"

namespace Ogre
{
	class SceneNode;
}

namespace ot::graphics::node
{
	void init_object(object& n, Ogre::SceneNode* node) noexcept;
	Ogre::SceneNode& get_scene_node(object& n) noexcept;
	Ogre::SceneNode const& get_scene_node(object const& n) noexcept;
}
