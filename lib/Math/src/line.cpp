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
}
