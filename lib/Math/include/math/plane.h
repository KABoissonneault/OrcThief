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

		[[nodiscard]] constexpr double distance_to(point3d p)
		{
			return normal.x * p.x + normal.y * p.y + normal.z * p.z - distance;
		}
	};

	enum class plane_side_result
	{
		on_plane,
		inside, // negative side
		outside, // positive side
	};
	
	[[nodiscard]] constexpr plane_side_result get_plane_side(plane p, point3d v)
	{
		double const distance = p.distance_to(v);
		if (distance > DBL_EPSILON) return plane_side_result::outside;
		else if (distance < -DBL_EPSILON) return plane_side_result::inside;
		else return plane_side_result::on_plane;
	}

	[[nodiscard]] constexpr std::optional<point3d> find_intersection(plane p1, plane p2, plane p3) noexcept
	{
		// Given
		//   w = a1*(b2*c3-b3*c2) + a2*(b3*c1-b1*c3) + a3*(b1*c2-b2*c1)
		// The intersection vertex will have:
		//   x = (d1*(b2*c3-b3*c2) + d2*(b3*c1-b1*c3) + d3*(b1*c2-b2*c1)) / w
		//   y = (c1*(a2*d3-a3*d2) + c2*(a3*d1-a1*d3) + c3*(a1*d2-a2*d1)) / w
		//   z = -(b1*(a2*d3-a3*d2) + b2*(a3*d1-a1*d3) + b3*(a1*d2-a2*d1)) / w
		// Where
		//   a(n), b(n), c(n) are the x, y, and z components of the normal of the Nth plane
		//   d(n) is the distance along the normal of the Nth plane
		
		const double bc12 = p1.normal.y * p2.normal.z - p2.normal.y * p1.normal.z; // b1*c2 - b2*c1
		const double bc23 = p2.normal.y * p3.normal.z - p3.normal.y * p2.normal.z; // b2*c3 - b3*c2
		const double bc31 = p3.normal.y * p1.normal.z - p1.normal.y * p3.normal.z; // b3*c1 - b1*c3
		
		const double w = (p1.normal.x * bc23 + p2.normal.x * bc31 + p3.normal.x * bc12);
		// const double w = -(p1.normal.x * mbc23 + p2.normal.x * mbc31 + p3.normal.x * mbc12);
		if (w > -DBL_EPSILON && w < DBL_EPSILON)
		{
			return {};
		}

		const double ad12 = p1.normal.x * p2.distance - p2.normal.x * p1.distance; // a1*d2 - a2*d1
		const double ad23 = p2.normal.x * p3.distance - p3.normal.x * p2.distance; // a2*d3 - a3*d2
		const double ad31 = p3.normal.x * p1.distance - p1.normal.x * p3.distance; // a3*d1 - a1*d3


		return point3d
		{
			(p1.distance * bc23 + p2.distance * bc31 + p3.distance * bc12) / w,
			(p1.normal.z * ad23 + p2.normal.z * ad31 + p3.normal.z * ad12) / w,
			-(p1.normal.y * ad23 + p2.normal.y * ad31 + p3.normal.y * ad12) / w
		};
	}
}
