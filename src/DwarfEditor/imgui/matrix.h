#pragma once

namespace ot::dedit::imgui
{
	// Unlike math::transform_matrix, this is column-major
	struct matrix
	{
		float elements[16];

		[[nodiscard]] static matrix identity() noexcept;
	};
}

namespace ot::math
{
	struct transform_matrix;
	dedit::imgui::matrix to_imgui(transform_matrix const& m);
}
