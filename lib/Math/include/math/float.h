#pragma once

#include <cfloat>
#include <cmath>

namespace ot::math
{
	[[nodiscard]] inline bool almost_equal(float lhs, float rhs) noexcept
	{
		return std::fabs(lhs - rhs) < FLT_EPSILON; // TODO: use relative error
	}

	[[nodiscard]] inline bool almost_equal(double lhs, double rhs) noexcept
	{
		return std::fabs(lhs - rhs) < DBL_EPSILON; // TODO: use relative error
	}
}