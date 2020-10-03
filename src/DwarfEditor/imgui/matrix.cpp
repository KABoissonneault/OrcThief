#include "imgui/matrix.h"
#include "imgui/boost/matrix_traits.h"

#include "math/transform_matrix.h"
#include "math/boost/vector_traits.h"
#include "math/boost/transform_matrix_traits.h"
#include "math/boost/transform_matrix_ops.h"
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
		return ot::math::ops::get_rotation(*this).to_quaternion();
	}

	math::scales matrix::get_scale() const noexcept
	{
		return ot::math::ops::get_scale(*this);
	}

	math::transform_matrix to_math_matrix(matrix const& m)
	{
		return boost::qvm::convert_to<math::transform_matrix>(m);
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
