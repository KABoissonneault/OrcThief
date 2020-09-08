#pragma once

#include "egfx/object/camera.h"

#include "Ogre/Prerequisites.h"

namespace ot::egfx::object
{
	[[nodiscard]] camera_cref make_camera_cref(Ogre::Camera const&) noexcept;
	[[nodiscard]] camera_ref make_camera_ref(Ogre::Camera&) noexcept;
	[[nodiscard]] camera make_camera(Ogre::Camera& c) noexcept;
	[[nodiscard]] Ogre::Camera const& get_camera(camera_cref) noexcept;
	[[nodiscard]] Ogre::Camera& get_camera(camera_ref) noexcept;
	[[nodiscard]] inline Ogre::Camera const& get_camera(camera const& o) noexcept { return get_camera(static_cast<camera_cref>(o)); }
	[[nodiscard]] inline Ogre::Camera& get_camera(camera& o) noexcept { return get_camera(static_cast<camera_ref>(o)); }
}