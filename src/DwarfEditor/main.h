#pragma once

#include "core/uptr.h"

#include "SDL2/window.h"
#include "Ogre/Prerequisites.h"

namespace ot::dedit::main
{
	[[nodiscard]] bool initialize();
	[[nodiscard]] int run(int argc, char** argv, sdl::unique_window window);
	void shutdown();
}
