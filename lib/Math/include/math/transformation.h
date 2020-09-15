#pragma once

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::math
{
	struct transformation
	{
		vector3f displacement;
		quaternion rotation;
		float scale;

		static transformation identity() noexcept { return { vector3f{0, 0, 0}, quaternion::identity(), 1.0 }; }
	};

	[[nodiscard]] inline transformation invert(transformation const& t) noexcept
	{
		return {
			-t.displacement,
			invert(t.rotation),
			1.0f / t.scale,
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

	[[nodiscard]] inline point3f transform(point3f p, transformation const& t) noexcept
	{
		vector3f const scaled_vector = vector_from_origin(p) * t.scale;
		vector3f const rotated_vector = rotate(scaled_vector, t.rotation);
		return destination_from_origin(rotated_vector + t.displacement);
	}

	[[nodiscard]] inline point3f detransform(point3f p, transformation const& t) noexcept
	{
		point3f const centered_point = p + t.displacement;
		vector3f const aligned_vector = rotate(vector_from_origin(centered_point), t.rotation);
		return destination_from_origin(aligned_vector * t.scale);
	}
}
