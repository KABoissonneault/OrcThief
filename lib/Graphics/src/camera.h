#pragma once

#include "graphics/camera.h"

#include "Ogre/Camera.h"

namespace ot::graphics
{
	class camera
	{
		Ogre::Camera* c = nullptr;

	public:
		camera() = default;
		explicit camera(Ogre::Camera* c) noexcept : c(c) {}
		camera& operator=(Ogre::Camera* rhs) noexcept
		{
			c = rhs;
			return *this;
		}

		Ogre::Camera* get() noexcept { return c; }
		Ogre::Camera const* get() const noexcept { return c; }
		Ogre::Camera* operator->() noexcept { return c; }
		Ogre::Camera const* operator->() const noexcept { return c; }
	};
}