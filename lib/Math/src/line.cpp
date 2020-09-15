#include "math/line.h"

namespace ot::math
{
	float distance(line const& l, point3f p)
	{
		vector3f const v1 = l.b - l.a;
		vector3f const v2 = l.a - p;

		vector3f const vr = cross_product(v1, v2);

		return vr.norm() / v1.norm();
	}

	line transform(line const& l, transformation const& t)
	{
		return {
			transform(l.a, t),
			transform(l.b, t)
		};
	}

	point3f lerp(line const& l, float f)
	{
		return l.a + (l.b - l.a) * f;
	}

	float inv_lerp(line const& l, point3f p)
	{
		vector3f const v1 = l.b - l.a;
		vector3f const v2 = p - l.a;

		return dot_product(v2, v1) / v1.norm_squared();
	}

	point3f clamp(line const& l, point3f p)
	{
		float const f = inv_lerp(l, p);
		float const df = std::clamp(f, 0.0f, 1.0f);
		return lerp(l, df);
	}

	point3f project(line const& l, point3f p)
	{
		math::vector3f const v1 = l.b - l.a;

		return l.a + v1 * inv_lerp(l, p);
	}

	point3f clamped_project(line const& l, point3f p)
	{
		vector3f const v1 = l.b - l.a;
		float const f = std::clamp(inv_lerp(l, p), 0.0f, 1.0f);
		return l.a + v1 * f;
	}
}
