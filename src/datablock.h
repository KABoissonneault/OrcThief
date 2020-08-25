#pragma once

#include <string>
#include <filesystem>

namespace ot::datablock
{
	void initialize(std::filesystem::path const& resource_folder);

	extern std::string const overlay_unlit;
	extern std::string const overlay_unlit_vertex;
	extern std::string const overlay_unlit_vertex_transparent;
	extern std::string const overlay_unlit_edge;
	extern std::string const overlay_unlit_transparent_light;
	extern std::string const overlay_unlit_transparent_heavy;
}
