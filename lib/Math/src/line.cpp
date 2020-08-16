#include "math/line.h"

namespace ot::math
{
	double distance(line const& l, point3d p)
	{
		math::vector3d const v1 = l.b - l.a;
		math::vector3d const v2 = l.a - p;

		math::vector3d const vr = cross_product(v1, v2);

		return vr.norm() / v1.norm();
	}

	line transform(line const& l, transformation const& t)
	{
		return {
			transform(l.a, t),
			transform(l.b, t)
		};
	}

	point3d lerp(line const& l, double f)
	{
		return l.a + (l.b - l.a) * f;
	}

	double inv_lerp(line const& l, point3d p)
	{
		math::vector3d const v1 = l.b - l.a;
		math::vector3d const v2 = p - l.a;

		return dot_product(v2, v1) / v1.norm_squared();
	}

	point3d clamp(line const& l, point3d p)
	{
		double const f = inv_lerp(l, p);
		double const df = std::clamp(f, 0.0, 1.0);
		return lerp(l, df);
	}

	point3d project(line const& l, point3d p)
	{
		math::vector3d const v1 = l.b - l.a;

		return l.a + v1 * inv_lerp(l, p);
	}

	point3d clamped_project(line const& l, point3d p)
	{
		math::vector3d const v1 = l.b - l.a;
		double const f = std::clamp(inv_lerp(l, p), 0.0, 1.0);
		return l.a + v1 * f;
	}
}
