#pragma once

#include "core/float.h"

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::math
{
	struct transform_matrix;
}

namespace ot::dedit::imgui
{
	// Unlike math::transform_matrix, this is column-major
	struct matrix
	{
		float elements[16];

		[[nodiscard]] static matrix identity() noexcept;

		math::vector3f get_displacement() const noexcept;
		math::quaternion get_rotation() const noexcept;
		float get_scale() const noexcept;
	};

	using ot::float_eq;
	[[nodiscard]] bool float_eq(matrix const& lhs, matrix const& rhs);
	[[nodiscard]] bool float_eq(math::transform_matrix const& lhs, matrix const& rhs);
	[[nodiscard]] bool float_eq(matrix const& lhs, math::transform_matrix const& rhs);
}

namespace ot::math
{
	dedit::imgui::matrix to_imgui(transform_matrix const& m);
}
