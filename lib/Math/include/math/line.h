#pragma once

#include "math/vector3.h"

namespace ot::math
{
	struct line
	{
		point3d a;
		point3d b;
	};

	// Returns only a positive distance
	double distance(line const& l, point3d p);
}
