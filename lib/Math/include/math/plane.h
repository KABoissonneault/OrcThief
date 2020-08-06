#pragma once

#include "math/vector3.h"

#include <optional>
#include <cfloat>

namespace ot::math
{
	// A plane is represented with a vector for its normal as an axis, and the distance along this axis
	struct plane
	{
		vector3d normal;
		double distance;

		[[nodiscard]] constexpr double distance_to(point3d p) const noexcept
		{
			return normal.x * p.x + normal.y * p.y + normal.z * p.z - distance;
		}

		// Returns a point on the plane
		[[nodiscard]] constexpr point3d get_point() const noexcept
		{
			return destination_from_origin(normal * distance);
		}
	};

	enum class plane_side_result
	{
		on_plane,
		inside, // negative side
		outside, // positive side
	};
	
	[[nodiscard]] plane_side_result get_plane_side(plane p, point3d v);
	[[nodiscard]] std::optional<point3d> find_intersection(plane p1, plane p2, plane p3);

	using ot::float_eq;
	[[nodiscard]] inline bool float_eq(plane const& lhs, plane const& rhs)
	{
		return float_eq(lhs.normal, rhs.normal) && float_eq(lhs.distance, rhs.distance);
	}
}
