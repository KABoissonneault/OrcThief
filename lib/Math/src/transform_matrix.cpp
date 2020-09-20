#include "math/transform_matrix.h"

#include "math/boost/detail/common.h"
#include "math/boost/vector_traits.h"
#include "math/boost/quaternion_traits.h"
#include "math/boost/transform_matrix_traits.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/mat_access.hpp>
#include <boost/qvm/mat.hpp>

#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/vec_mat_operations.hpp>

#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/quat_access.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

#include <cassert>

namespace ot::math
{
	float rotation_matrix::determinant() const noexcept
	{
		return boost::qvm::determinant(*this);
	}

	rotation_matrix rotation_matrix::rotx(float radiant) noexcept
	{
		return boost::qvm::rotx_mat<3>(radiant);
	}

	rotation_matrix rotation_matrix::roty(float radiant) noexcept
	{
		return boost::qvm::roty_mat<3>(radiant);
	}

	rotation_matrix rotation_matrix::rotz(float radiant) noexcept
	{
		return boost::qvm::rotz_mat<3>(radiant);
	}

	rotation_matrix rotation_matrix::identity() noexcept
	{
		return boost::qvm::identity_mat<float, 3>();
	}

	quaternion rotation_matrix::to_quaternion() const noexcept
	{
		return boost::qvm::convert_to<quaternion>(*this);
	}

	rotation_matrix invert(rotation_matrix const& t)
	{
		return boost::qvm::transposed(t);
	}

	rotation_matrix operator*(rotation_matrix const& lhs, rotation_matrix const& rhs)
	{
		return boost::qvm::operator*(lhs, rhs);
	}

	bool float_eq(rotation_matrix const& lhs, rotation_matrix const& rhs) noexcept
	{
		return boost::qvm::cmp(lhs, rhs, [](float l, float r) 
		{ 
			return float_eq(l, r); 
		});
	}

	vector3f rotate(vector3f v, rotation_matrix const& r) noexcept
	{
		using boost::qvm::operator*;
		return r * v;
	}

	transform_matrix transform_matrix::identity()
	{
		return boost::qvm::identity_mat<float, 4>();
	}

	transform_matrix transform_matrix::from_components(vector3f const& displacement, quaternion const& rotation, float scale)
	{
		using boost::qvm::operator*=;

		auto mat = boost::qvm::convert_to<transform_matrix>(rotation);
		boost::qvm::del_row_col<3, 3>(mat) *= scale;
		boost::qvm::translation(mat) = displacement;		

		return mat;
	}

	transform_matrix transform_matrix::from_components(vector3f const& displacement, rotation_matrix const& rotation, float scale)
	{
		using boost::qvm::operator*=;
		using boost::qvm::operator*;

		transform_matrix mat = boost::qvm::identity_mat<float, 4>();
		auto& mat_rotation = boost::qvm::del_row_col<3, 3>(mat);
		mat_rotation = rotation;

		rotation_matrix const identity = boost::qvm::identity_mat<float, 3>();
		mat_rotation *= identity * scale;

		boost::qvm::translation(mat) = displacement;

		return mat;
	}

	vector3f transform_matrix::get_displacement() const noexcept
	{
		return boost::qvm::translation(*this);
	}

	float transform_matrix::get_scale() const noexcept
	{
		auto const& rotation_mat = boost::qvm::del_row_col<3, 3>(*this);
		assert(
			float_eq(boost::qvm::mag(boost::qvm::col<0>(rotation_mat)), boost::qvm::mag(boost::qvm::col<1>(rotation_mat)), 4)
			&& float_eq(boost::qvm::mag(boost::qvm::col<0>(rotation_mat)), boost::qvm::mag(boost::qvm::col<2>(rotation_mat)), 4)
			&& "Scaling is not uniform!"
		);
		return boost::qvm::mag(boost::qvm::col<0>(rotation_mat));
	}

	rotation_matrix transform_matrix::get_rotation() const noexcept
	{
		using boost::qvm::operator*;
		float const scale = get_scale();
		return boost::qvm::convert_to<rotation_matrix>(boost::qvm::del_row_col<3, 3>(*this) * (1.0f / scale));
	}

	transform_matrix invert(transform_matrix const& i)
	{
		return boost::qvm::inverse(i);
	}

	transform_matrix operator*(transform_matrix const& lhs, transform_matrix const& rhs)
	{
		return boost::qvm::operator*(lhs, rhs);
	}

	point3f transform(point3f p, transform_matrix const& t) noexcept
	{
		return boost::qvm::transform_point(t, p);
	}

	bool float_eq(transform_matrix const& lhs, transform_matrix const& rhs) noexcept
	{
		return boost::qvm::cmp(lhs, rhs, [] (float l, float r)
		{
			return float_eq(l, r);
		});
	}
}
