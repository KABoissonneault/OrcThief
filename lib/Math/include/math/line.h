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

	point3d lerp(line const& l, double f);
	double inv_lerp(line const& l, point3d p);
	point3d clamp(line const& l, point3d p);

	// Returns a point on the line
	point3d project(line const& l, point3d p);

	// Returns a point on the line between the two points
	point3d clamped_project(line const& l, point3d p);

	line transform(line const& l, transformation const& t);
}
