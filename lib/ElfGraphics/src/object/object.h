#pragma once

#include "egfx/object/object.h"

#include "Ogre/Prerequisites.h"

namespace ot::egfx
{
	void init_object_cref(object_cref&, Ogre::MovableObject const&) noexcept;
	void init_object_ref(object_ref&, Ogre::MovableObject&) noexcept;
	[[nodiscard]] object_cref make_object_cref(Ogre::MovableObject const&) noexcept;
	[[nodiscard]] object_ref make_object_ref(Ogre::MovableObject&) noexcept;
	[[nodiscard]] Ogre::MovableObject const& get_object(object_cref) noexcept;
	[[nodiscard]] Ogre::MovableObject& get_object(object_ref) noexcept;

	[[nodiscard]] object_type get_object_type(Ogre::MovableObject const&);
	[[nodiscard]] bool is_object_of_type(Ogre::MovableObject const&, object_type);
}
