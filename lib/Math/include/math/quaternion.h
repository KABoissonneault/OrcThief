#pragma once

#include "math/vector3.h"

namespace ot::math
{
	struct quaternion
	{
		double w, x, y, z;

		static quaternion make_rotx(double angle);
		static quaternion make_roty(double angle);
		static quaternion make_rotz(double angle);
	};

	[[nodiscard]] vector3d rotate(vector3d v, quaternion q);
}
