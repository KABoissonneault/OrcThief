#pragma once

#include "graphics/node/object.h"

namespace Ogre
{
	class SceneNode;
}

namespace ot::graphics::node
{
	object_cref make_object_cref(Ogre::SceneNode const&) noexcept;
	object_ref make_object_ref(Ogre::SceneNode&) noexcept;
	Ogre::SceneNode const& get_scene_node(object_cref) noexcept;
	Ogre::SceneNode& get_scene_node(object_ref) noexcept;
	inline Ogre::SceneNode const& get_scene_node(object const& o) noexcept { return get_scene_node(static_cast<object_cref>(o)); }
	inline Ogre::SceneNode& get_scene_node(object& o) noexcept { return get_scene_node(static_cast<object_ref>(o)); }
}
