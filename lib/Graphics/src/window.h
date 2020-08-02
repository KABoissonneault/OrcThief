#pragma once

#include "graphics/window.h"
#include "graphics/window_type.h"

#include "Ogre/Prerequisites.h"

namespace ot::graphics
{
	class window
	{
		Ogre::Window* render;
		window_id id;

	public:
		window() = default;
		window(Ogre::Window* render, window_id id) noexcept;

		Ogre::Window& get_window() const noexcept { return *render; }
		window_id get_id() const noexcept { return id; }

		int get_width() const noexcept;
		int get_height() const noexcept;
	};
}