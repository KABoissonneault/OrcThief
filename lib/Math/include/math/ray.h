#pragma once

#include "math/vector3.h"
#include "math/plane.h"

#include <optional>

namespace ot::math
{
	struct ray
	{
		point3f origin;
		vector3f direction;

		std::optional<point3f> intersects(plane p) const noexcept;
	};
}
