#pragma once

#include "math/vector3.h"
#include "math/transform_matrix.h"

namespace ot::math
{
	struct line
	{
		point3f a;
		point3f b;
	};

	// Returns only a positive distance
	float distance(line const& l, point3f p);

	point3f lerp(line const& l, float f);
	float inv_lerp(line const& l, point3f p);
	point3f clamp(line const& l, point3f p);

	// Returns a point on the line
	point3f project(line const& l, point3f p);

	// Returns a point on the line between the two points
	point3f clamped_project(line const& l, point3f p);

	line transform(line const& l, transform_matrix const& t);
}
