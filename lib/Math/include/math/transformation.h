#pragma once

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::math
{
	struct transformation
	{
		vector3d displacement;
		quaternion rotation;
		double scale;

		static transformation identity() noexcept { return { vector3d{0, 0, 0}, quaternion::identity(), 1.0 }; }
	};

	[[nodiscard]] inline transformation invert(transformation const& t) noexcept
	{
		return {
			-t.displacement,
			invert(t.rotation),
			1.0 / t.scale,
		};
	}

	[[nodiscard]] inline transformation concatenate(transformation const& lhs, transformation const& rhs) noexcept
	{
		return {
			lhs.displacement + rhs.displacement,
			concatenate(lhs.rotation, rhs.rotation),
			lhs.scale * rhs.scale
		};
	}

	[[nodiscard]] inline point3d transform(point3d p, transformation const& t) noexcept
	{
		vector3d const scaled_vector = vector_from_origin(p) * t.scale;
		vector3d const rotated_vector = rotate(scaled_vector, t.rotation);
		return destination_from_origin(rotated_vector + t.displacement);
	}

	[[nodiscard]] inline point3d detransform(point3d p, transformation const& t) noexcept
	{
		point3d const centered_point = p + t.displacement;
		vector3d const aligned_vector = rotate(vector_from_origin(centered_point), t.rotation);
		return destination_from_origin(aligned_vector * t.scale);
	}
}
