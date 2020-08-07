#include "math/plane.h"

namespace ot::math
{
	plane_side_result get_plane_side(plane p, point3d v)
	{
		double const distance = p.distance_to(v);
		auto const cmp = float_cmp(distance, 0.0);
		if (cmp > 0)
			return plane_side_result::outside;
		else if (cmp < 0)
			return plane_side_result::inside;
		else 
			return plane_side_result::on_plane;
	}

	std::optional<point3d> find_intersection(plane p1, plane p2, plane p3)
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
		if (float_eq(w, 0.0))
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

	plane transform(plane p, quaternion rotation, vector3d displacement)
	{
		p.normal = rotate(p.normal, rotation);
		p.distance += dot_product(p.normal, displacement);
		return p;
	}
}
