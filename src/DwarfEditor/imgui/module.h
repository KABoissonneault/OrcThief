#pragma once

#include <filesystem>

struct SDL_Window;

namespace ImGui
{
	struct MarkdownConfig;
}

namespace ot::dedit
{
	class config;
}

namespace ot::dedit::imgui
{
	// Called once, at the start of the application
	// Config must be valid until imgui::shutdown is called
	bool initialize(SDL_Window& window, config const& config);
	// Called every time the fonts need to be rebuilt
	bool build_fonts();

	// Called after event handling, before update
	void pre_update();
	// Called after rendering, before the next frame
	void end_frame();
	
	// End the use of imgui
	void shutdown();

	// Utility
	bool has_mouse();

	void initialize_markdown(ImGui::MarkdownConfig& config);

	std::string get_doc_path(std::string_view doc_name);
}
