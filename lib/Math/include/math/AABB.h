#pragma once

#include "math/vector3.h"

#include <numeric>

namespace ot::math
{
	// Axis-aligned bounding box
	struct aabb
	{
		point3d position;
		vector3d half_size; // 3 positive values, each indicating half the size along the equivalent axis

		// Returns the "max" point in the box, being the point where each coordinate is at its highest
		point3d max() const noexcept { return position + half_size; }
		// Returns the "min" point in the box, being the point where each coordinate is at its lowest
		point3d min() const noexcept { return position - half_size; }

		// Expand the box to include the extent of the other box
		void merge(aabb const& rhs)
		{
			point3d const min = component_min(this->min(), rhs.min());
			point3d const max = component_max(this->max(), rhs.max());
			position = midpoint(min, max);
			half_size = (max - min) * 0.5;
		}

		// Expand the box to include the point
		void merge(point3d const& rhs)
		{
			point3d const min = component_min(this->min(), rhs);
			point3d const max = component_max(this->max(), rhs);
			position = midpoint(min, max);
			half_size = (max - min) * 0.5;
		}
	};

	
}
