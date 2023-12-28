#pragma once

#include "core/uptr.h"

#include "SDL2/window.h"
#include "Ogre/Prerequisites.h"

namespace ot::dedit::main
{
	[[nodiscard]] bool initialize(int argc, char** argv);
	[[nodiscard]] int run(int argc, char** argv, sdl::unique_window window);
	void shutdown();
}
