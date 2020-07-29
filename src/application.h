#pragma once

#include <memory>

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "graphics/module.h"

namespace ot
{
	class application
	{
		sdl::unique_window main_window;
		graphics::module graphics;
		graphics::module::unique_scene main_scene;

	public:
		bool initialize();
		void setup_default_scene();

		void run();
	};
}