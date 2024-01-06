#pragma once

#include "egfx/node.h"

#include "Ogre/Prerequisites.h"

namespace ot::egfx
{
	[[nodiscard]] node_cref make_node_cref(Ogre::SceneNode const&) noexcept;
	[[nodiscard]] node_ref make_node_ref(Ogre::SceneNode&) noexcept;
	[[nodiscard]] Ogre::SceneNode const& get_scene_node(node_cref) noexcept;
	[[nodiscard]] Ogre::SceneNode& get_scene_node(node_ref) noexcept;
	[[nodiscard]] inline Ogre::SceneNode const& get_scene_node(node const& o) noexcept { return get_scene_node(static_cast<node_cref>(o)); }
	[[nodiscard]] inline Ogre::SceneNode& get_scene_node(node& o) noexcept { return get_scene_node(static_cast<node_ref>(o)); }
}
