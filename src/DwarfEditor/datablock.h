#pragma once

#include <string>
#include <filesystem>

namespace ot::dedit::datablock
{
	void load_hlms(std::filesystem::path const& resource_folder);

	void create_default_materials();
}
