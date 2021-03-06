#pragma once

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <optional>
#include <cfloat>

namespace ot::math
{
	// A plane is represented with a vector for its normal as an axis, and the distance along this axis
	struct plane
	{
		vector3f normal;
		float distance;

		[[nodiscard]] constexpr float distance_to(point3f p) const noexcept
		{
			return normal.x * p.x + normal.y * p.y + normal.z * p.z - distance;
		}

		// Returns a point on the plane
		[[nodiscard]] constexpr point3f get_point() const noexcept
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
	
	[[nodiscard]] plane_side_result get_plane_side(plane p, point3f v);
	[[nodiscard]] plane_side_result distance_to_plane_side(float distance);

	[[nodiscard]] std::optional<point3f> find_intersection(plane p1, plane p2, plane p3);
	[[nodiscard]] point3f find_distance_ray_intersection(point3f v1, float d1, point3f v2, float d2);

	[[nodiscard]] plane transform(plane p, transform_matrix const& t);

	using ot::float_eq;
	[[nodiscard]] inline bool float_eq(plane const& lhs, plane const& rhs)
	{
		return float_eq(lhs.normal, rhs.normal) && float_eq(lhs.distance, rhs.distance);
	}
}
