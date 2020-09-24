#pragma once

#include "math/vector3.h"
#include "math/quaternion.h"
#include "core/float.h"

namespace ot::math
{
	struct rotation_matrix
	{
		float elements[9];

		[[nodiscard]] float determinant() const noexcept;

		[[nodiscard]] static rotation_matrix identity() noexcept;
		[[nodiscard]] static rotation_matrix rotx(float radiant) noexcept;
		[[nodiscard]] static rotation_matrix roty(float radiant) noexcept;
		[[nodiscard]] static rotation_matrix rotz(float radiant) noexcept;

		[[nodiscard]] quaternion to_quaternion() const noexcept;
	};

	[[nodiscard]] rotation_matrix invert(rotation_matrix const& t);
	[[nodiscard]] rotation_matrix operator*(rotation_matrix const& lhs, rotation_matrix const& rhs);
	[[nodiscard]] bool float_eq(rotation_matrix const& lhs, rotation_matrix const& rhs) noexcept;

	[[nodiscard]] vector3f rotate(vector3f v, rotation_matrix const& r) noexcept;

	struct transform_matrix
	{
		float elements[16];

		[[nodiscard]] vector3f get_displacement() const noexcept;
		[[nodiscard]] rotation_matrix get_rotation() const noexcept;
		[[nodiscard]] float get_scale() const noexcept;

		[[nodiscard]] static transform_matrix identity();
		[[nodiscard]] static transform_matrix from_components(vector3f const& displacement, quaternion const& rotation, float scale = 1.0f);
		[[nodiscard]] static transform_matrix from_components(vector3f const& displacement, rotation_matrix const& rotation, float scale = 1.0f);
	};

	[[nodiscard]] transform_matrix invert(transform_matrix const& t);
	[[nodiscard]] transform_matrix operator*(transform_matrix const& lhs, transform_matrix const& rhs);
	[[nodiscard]] point3f transform(point3f p, transform_matrix const& t) noexcept;

	[[nodiscard]] bool float_eq(transform_matrix const& lhs, transform_matrix const& rhs) noexcept;
}