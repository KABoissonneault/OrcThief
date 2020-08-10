#pragma once

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::math
{
	struct transformation
	{
		vector3d displacement;
		quaternion rotation;

		static transformation identity() noexcept { return { vector3d{0, 0, 0}, quaternion::identity() }; }
	};

	[[nodiscard]] inline transformation invert(transformation const& t) noexcept
	{
		return {
			-t.displacement,
			invert(t.rotation)
		};
	}

	[[nodiscard]] inline transformation concatenate(transformation const& lhs, transformation const& rhs) noexcept
	{
		return {
			lhs.displacement + rhs.displacement,
			concatenate(lhs.rotation, rhs.rotation)
		};
	}

	[[nodiscard]] inline point3d transform(point3d p, transformation const& t) noexcept
	{
		vector3d const rotated_vector = rotate(vector_from_origin(p), t.rotation);
		return destination_from_origin(rotated_vector + t.displacement);
	}
}
