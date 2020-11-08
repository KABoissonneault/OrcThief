#pragma once

#include <filesystem>

struct SDL_Window;

namespace ot::dedit::imgui
{
	// Called once, at the start of the application
	bool initialize(SDL_Window& window);
	// Called every time the fonts need to be rebuilt
	bool build_fonts(std::filesystem::path const& resource_folder);
	// Called after event handling, before update
	void pre_update(SDL_Window& window);
	// Called after rendering, before the next frame
	void end_frame();
	void shutdown();

	bool has_mouse();
}
