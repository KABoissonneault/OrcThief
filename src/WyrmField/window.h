#pragma once

#include "SDL2/window.h"

namespace ot::wf
{
	[[nodiscard]] sdl::unique_window create_window(char const* window_title);
}
