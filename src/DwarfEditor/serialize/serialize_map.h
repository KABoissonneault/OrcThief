#pragma once

#include "map.h"

#include <cstdio>

namespace ot::dedit::serialize
{
	bool fwrite(map const& m, std::FILE* stream);
	bool fread(map& m, std::FILE* stream);
}
