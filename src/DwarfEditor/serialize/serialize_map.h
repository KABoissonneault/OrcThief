#pragma once

#include "map.h"

#include <cstdio>
#include <optional>

namespace ot::dedit::serialize
{
	bool fwrite(map const& m, std::FILE* stream);
	bool fread(map& m, std::FILE* stream);

	bool fwrite(map_entity const& e, std::FILE* f);
	bool fread(map& m, map_entity& parent, std::FILE* f, map_entity** new_entity = nullptr);
}
