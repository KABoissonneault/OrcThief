#pragma once

#include "math/vector3.h"

namespace ot::graphics
{
	struct ray
	{
		math::point3d origin;
		math::vector3d direction;
	};
}
