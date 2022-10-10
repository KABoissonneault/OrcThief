#pragma once

struct SDL_Window;

namespace ot::wf::imgui
{
	bool initialize(SDL_Window& window);

	void pre_update();

	void end_frame();

	void shutdown();
}
