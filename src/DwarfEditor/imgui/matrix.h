#pragma once

#include "core/float.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <span>
#include <string_view>

namespace ot::dedit::imgui
{
	// Unlike math::transform_matrix, this is column-major
	struct matrix
	{
		float elements[16];

		[[nodiscard]] static matrix identity() noexcept;
		[[nodiscard]] static matrix from_components(math::vector3f displacement, math::quaternion rotation, math::scales scales = { 1.f, 1.f, 1.f }) noexcept;

		[[nodiscard]] math::vector3f get_displacement() const noexcept;
		[[nodiscard]] math::quaternion get_rotation() const noexcept;
		[[nodiscard]] math::scales get_scale() const noexcept;

		void decompose(std::span<float, 3> displacement, std::span<float, 3> euler_rotation, std::span<float, 3> scales) const;
		void recompose(std::span<float const, 3> displacement, std::span<float const, 3> euler_rotation, std::span<float const, 3> scales);
	};

	[[nodiscard]] math::transform_matrix to_math_matrix(matrix const& m);

	[[nodiscard]] matrix invert(matrix const& t);
	[[nodiscard]] matrix operator*(matrix const& lhs, matrix const& rhs);

	using ot::float_eq;
	[[nodiscard]] bool float_eq(matrix const& lhs, matrix const& rhs);
	[[nodiscard]] bool float_eq(math::transform_matrix const& lhs, matrix const& rhs);
	[[nodiscard]] bool float_eq(matrix const& lhs, math::transform_matrix const& rhs);

	void text(matrix const& m);
	void text(matrix const& m, std::string_view title);
}

namespace ot::math
{
	dedit::imgui::matrix to_imgui(transform_matrix const& m);
}
