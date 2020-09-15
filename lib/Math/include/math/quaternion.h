#pragma once

#include "math/vector3.h"

#include "core/float.h"

namespace ot::math
{
	struct quaternion
	{
		float w, x, y, z;

		[[nodiscard]] static quaternion identity() noexcept;
		[[nodiscard]] static quaternion x_rad_rotation(float angle) noexcept;
		[[nodiscard]] static quaternion y_rad_rotation(float angle) noexcept;
		[[nodiscard]] static quaternion z_rad_rotation(float angle) noexcept;
		[[nodiscard]] static quaternion x_deg_rotation(float angle) noexcept;
		[[nodiscard]] static quaternion y_deg_rotation(float angle) noexcept;
		[[nodiscard]] static quaternion z_deg_rotation(float angle) noexcept;
	};

	[[nodiscard]] quaternion invert(quaternion const& q) noexcept;
	[[nodiscard]] quaternion concatenate(quaternion const& lhs, quaternion const& rhs) noexcept;
	template<typename... Args>
	[[nodiscard]] inline quaternion concatenate(quaternion const& lhs, quaternion const& rhs, Args const&... args) noexcept
	{
		return concatenate(concatenate(lhs, rhs), args...);
	}

	[[nodiscard]] quaternion operator*(quaternion const& q, double m) noexcept;

	[[nodiscard]] vector3f rotate(vector3f v, quaternion q) noexcept;

	using ot::float_eq;
	[[nodiscard]] bool float_eq(quaternion const& lhs, quaternion const& rhs);
}
