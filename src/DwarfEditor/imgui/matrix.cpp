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

	void matrix::decompose(std::span<float, 3> displacement, std::span<float, 3> euler_rotation, std::span<float, 3> scales) const
	{
		math::scales const s = get_scale();
		scales[0] = s.x;
		scales[1] = s.y;
		scales[2] = s.z;

		math::rotation_matrix const r = math::ops::get_rotation(*this, s);
		euler_rotation[0] = math::ops::get_rotx(r);
		euler_rotation[1] = math::ops::get_roty(r);
		euler_rotation[2] = math::ops::get_rotz(r);

		math::vector3f const d = get_displacement();
		displacement[0] = d.x;
		displacement[1] = d.y;
		displacement[2] = d.z;
	}

	void matrix::recompose(std::span<float const, 3> displacement, std::span<float const, 3> euler_rotation, std::span<float const, 3> scales)
	{
		using namespace boost::qvm;
		using boost::qvm::operator*;

		math::vector3f const d{ displacement[0], displacement[1], displacement[2] };
		auto const r = rot_mat_xyz<3>(euler_rotation[0], euler_rotation[1], euler_rotation[2]);
		math::scales s{ scales[0], scales[1], scales[2] };
				
		assign(
			del_row_col<3, 3>(*this), 
			r * diag_mat(s)
		);

		assign(translation(*this), d);
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
