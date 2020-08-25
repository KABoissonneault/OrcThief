#pragma once

struct SDL_Window;

namespace ot
{
	bool initialize_imgui(SDL_Window& window);
	void shutdown_imgui();
}
