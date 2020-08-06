#pragma once

#include "math/vector3.h"
#include "math/plane.h"

#include <optional>

namespace ot::math
{
	struct ray
	{
		point3d origin;
		vector3d direction;

		std::optional<point3d> intersects(plane p) const noexcept;
	};
}
