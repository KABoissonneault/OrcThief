#pragma once

#include "map.h"

#include <iosfwd>

namespace ot::dedit
{
	std::ostream& operator<<(std::ostream& o, map const& m);
	std::istream& operator>>(std::istream& i, map& m);
}
