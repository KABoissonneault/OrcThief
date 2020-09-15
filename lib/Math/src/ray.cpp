#include "math/ray.h"

#include "core/float.h"

namespace ot::math
{
	// d = dot(p0 - l0, p_n) / dot(l_n, p_n)
	// c = d*l_n
	// if dot(l_n, p_n) == 0, no collision
	std::optional<point3f> ray::intersects(plane p) const noexcept
	{
		float const denominator = dot_product(this->direction, p.normal);
		if (float_eq(denominator, 0.0f))
		{
			return {};
		}

		float const numerator = dot_product(p.get_point() - this->origin, p.normal);
		float const d = numerator / denominator;
		return this->origin + this->direction * d;
	}
}
