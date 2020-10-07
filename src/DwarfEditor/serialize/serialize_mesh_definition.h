#pragma once

#include "egfx/mesh_definition.fwd.h"
#include <cstdio>

namespace ot::dedit::serialize
{
	bool fwrite(egfx::mesh_definition const& m, std::FILE* f);
	bool fread(egfx::mesh_definition& m, std::FILE* f);
}
