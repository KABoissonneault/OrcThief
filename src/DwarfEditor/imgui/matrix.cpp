#include "imgui/matrix.h"
#include "imgui/boost/matrix_traits.h"

#include "math/transform_matrix.h"
#include "math/boost/vector_traits.h"
#include "math/boost/transform_matrix_traits.h"
#include "math/boost/quaternion_traits.h"

OT_IMGUI_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/mat.hpp>

#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/map_mat_vec.hpp>

#include <boost/qvm/quat_operations.hpp>
OT_IMGUI_DETAIL_BOOST_INCLUDE_END

namespace ot::dedit::imgui
{
	matrix matrix::identity() noexcept
	{
		return boost::qvm::identity_mat<float, 4>();
	}

	math::vector3f matrix::get_displacement() const noexcept
	{
		return boost::qvm::translation(*this);
	}

	math::quaternion matrix::get_rotation() const noexcept
	{
		using boost::qvm::operator*;
		float const scale = get_scale();
		return boost::qvm::convert_to<math::quaternion>(boost::qvm::del_row_col<3, 3>(*this) * (1.0f / scale));
	}

	float matrix::get_scale() const noexcept
	{
		auto const& rotation_mat = boost::qvm::del_row_col<3, 3>(*this);

		float const scale_x = boost::qvm::mag(boost::qvm::col<0>(rotation_mat));

#if !defined(NDEBUG) || !NDEBUG
		float const scale_y = boost::qvm::mag(boost::qvm::col<1>(rotation_mat));
		float const scale_z = boost::qvm::mag(boost::qvm::col<2>(rotation_mat));
		assert(
			float_eq(scale_x, scale_y, 4) && float_eq(scale_x, scale_z, 4)
			&& "Scaling is not uniform!"
		);
#endif

		return scale_x;
	}

	bool float_eq(matrix const& lhs, matrix const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r)
		{
			return float_eq(l, r);
		});
	
	}
	bool float_eq(math::transform_matrix const& lhs, matrix const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r)
		{
			return float_eq(l, r);
		});
	}

	bool float_eq(matrix const& lhs, math::transform_matrix const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r)
		{
			return float_eq(l, r);
		});
	}
}

namespace ot::math
{
	dedit::imgui::matrix to_imgui(transform_matrix const& m)
	{
		return boost::qvm::convert_to<dedit::imgui::matrix>(m);
	}
}
