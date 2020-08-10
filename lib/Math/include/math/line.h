#pragma once

#include "math/vector3.h"
#include "math/transformation.h"

namespace ot::math
{
	struct line
	{
		point3d a;
		point3d b;
	};

	// Returns only a positive distance
	double distance(line const& l, point3d p);

	line transform(line const& l, transformation const& t);
}
