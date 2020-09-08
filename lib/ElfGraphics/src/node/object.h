#pragma once

#include "egfx/node/object.h"

#include "Ogre/Prerequisites.h"

namespace ot::egfx::node
{
	[[nodiscard]] object_cref make_object_cref(Ogre::SceneNode const&) noexcept;
	[[nodiscard]] object_ref make_object_ref(Ogre::SceneNode&) noexcept;
	[[nodiscard]] Ogre::SceneNode const& get_scene_node(object_cref) noexcept;
	[[nodiscard]] Ogre::SceneNode& get_scene_node(object_ref) noexcept;
	[[nodiscard]] inline Ogre::SceneNode const& get_scene_node(object const& o) noexcept { return get_scene_node(static_cast<object_cref>(o)); }
	[[nodiscard]] inline Ogre::SceneNode& get_scene_node(object& o) noexcept { return get_scene_node(static_cast<object_ref>(o)); }
}
