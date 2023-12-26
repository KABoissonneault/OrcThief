#include "math/transform_matrix.h"

#include "math/boost/detail/common.h"
#include "math/boost/vector_traits.h"
#include "math/boost/quaternion_traits.h"
#include "math/boost/transform_matrix_traits.h"
#include "math/boost/transform_matrix_ops.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/mat_access.hpp>
#include <boost/qvm/mat.hpp>

#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/vec_mat_operations.hpp>
#include <boost/qvm/swizzle.hpp>

#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/quat_access.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

#include <numbers>

namespace ot::math
{
	float rotation_matrix::determinant() const noexcept
	{
		return boost::qvm::determinant(*this);
	}

	euler_rotation rotation_matrix::get_euler_angles_zyx() const noexcept
	{
		using namespace boost::qvm;

		euler_rotation e;

		// https://www.geometrictools.com/Documentation/EulerAngles.pdf
		// A20 is equal to -sin(y)
		// Therefore, if A20 == 1.f, then sin(y) == -1.f, or y = -pi/2
		if (float_eq(A20(*this), 1.f))
		{
			e.x = std::atan2(-A12(*this), A11(*this));
			e.y = -std::numbers::pi_v<float> / 2.f;
			e.z = 0.f;
		}
		else if (float_eq(A20(*this), -1.f))
		{
			e.x = std::atan2(-A12(*this), A11(*this));
			e.y = std::numbers::pi_v<float> / 2.f;
			e.z = 0.f;
		}
		else
		{
			e.x = std::atan2(A21(*this), A22(*this));
			e.y = std::asin(-A20(*this));
			e.z = std::atan2(A10(*this), A00(*this));
		}

		return e;
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

	rotation_matrix rotation_matrix::rot_zyx(euler_rotation const& e) noexcept
	{
		return boost::qvm::rot_mat_zyx<3>(e.z, e.y, e.x);
	}

	rotation_matrix rotation_matrix::identity() noexcept
	{
		return boost::qvm::identity_mat<float, 3>();
	}

	rotation_matrix rotation_matrix::from_quaternion(quaternion q) noexcept
	{
		return boost::qvm::convert_to<rotation_matrix>(q);
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

	bool float_eq(scales const& lhs, scales const& rhs)
	{
		return float_eq(lhs.x, rhs.x)
			&& float_eq(lhs.y, rhs.y)
			&& float_eq(lhs.z, rhs.z);
	}

	transform_matrix transform_matrix::identity()
	{
		return boost::qvm::identity_mat<float, 4>();
	}

	transform_matrix transform_matrix::from_components(vector3f const& displacement, quaternion const& rotation, float scale)
	{
		return from_components(displacement, rotation, { scale, scale, scale });
	}

	transform_matrix transform_matrix::from_components(vector3f const& displacement, quaternion const& rotation, math::scales const& scale)
	{
		using boost::qvm::operator*=;

		auto mat = boost::qvm::convert_to<transform_matrix>(rotation);
		boost::qvm::del_row_col<3, 3>(mat) *= boost::qvm::diag_mat(scale);
		boost::qvm::translation(mat) = displacement;

		return mat;
	}

	transform_matrix transform_matrix::from_components(vector3f const& displacement, rotation_matrix const& rotation, float scale)
	{
		return from_components(displacement, rotation, { scale, scale, scale });
	}

	transform_matrix transform_matrix::from_components(vector3f const& displacement, rotation_matrix const& rotation, math::scales const& scale)
	{
		using boost::qvm::operator*=;

		transform_matrix mat = boost::qvm::identity_mat<float, 4>();
		auto& mat_rotation = boost::qvm::del_row_col<3, 3>(mat);
		mat_rotation = rotation;
		mat_rotation *= boost::qvm::diag_mat(scale);

		boost::qvm::translation(mat) = displacement;

		return mat;
	}

	vector3f transform_matrix::get_displacement() const noexcept
	{
		return boost::qvm::translation(*this);
	}
			
	scales transform_matrix::get_scale() const noexcept
	{
		return ops::get_scale(*this);
	}

	rotation_matrix transform_matrix::get_rotation() const noexcept
	{
		return ops::get_rotation(*this);
	}

	void transform_matrix::set_displacement(vector3f const& dis)
	{
		boost::qvm::translation(*this) = dis;
	}

	void transform_matrix::rotate_x(float rad)
	{
		boost::qvm::rotate_x(*this, rad);
	}

	void transform_matrix::rotate_y(float rad)
	{
		boost::qvm::rotate_y(*this, rad);
	}

	void transform_matrix::rotate_z(float rad)
	{
		boost::qvm::rotate_z(*this, rad);
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

	vector3f transform(vector3f p, transform_matrix const& t) noexcept
	{
		return boost::qvm::transform_vector(t, p);
	}

	bool float_eq(transform_matrix const& lhs, transform_matrix const& rhs) noexcept
	{
		return boost::qvm::cmp(lhs, rhs, [] (float l, float r)
		{
			return float_eq(l, r);
		});
	}
}
