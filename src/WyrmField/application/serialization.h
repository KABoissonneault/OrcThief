#pragma once

#include <iosfwd>
#include <span>
#include <vector>

namespace ot::wf
{
	namespace m3
	{
		struct enemy_template;
	}

	void save_enemy_template(std::ostream& o, std::span<m3::enemy_template> e);
	std::vector<m3::enemy_template> load_enemy_template(std::istream& i);
}
