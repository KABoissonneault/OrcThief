#pragma once

struct SDL_Window;

namespace ot::dedit::imgui
{
	bool initialize(SDL_Window& window);
	void new_frame(SDL_Window& window);
	void render();
	void end_frame();
	void shutdown();

	bool has_mouse();
}
