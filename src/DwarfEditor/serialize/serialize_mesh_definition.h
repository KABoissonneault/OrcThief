#pragma once

#include "egfx/mesh_definition.fwd.h"
#include <iosfwd>

namespace ot::dedit
{
	std::ostream& operator<<(std::ostream& o, egfx::mesh_definition const& m);
	std::istream& operator>>(std::istream& i, egfx::mesh_definition& m);
}
