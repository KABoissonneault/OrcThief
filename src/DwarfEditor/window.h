#pragma once

#include "SDL2/window.h"

namespace ot::dedit
{
	sdl::unique_window create_window(char const* window_title);
}
