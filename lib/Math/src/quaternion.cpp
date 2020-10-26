#include "math/quaternion.h"

#include "math/boost/detail/common.h"
#include "math/boost/vector_traits.h"
#include "math/boost/quaternion_traits.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/quat_vec_operations.hpp>

#include <boost/qvm/quat_vec_operations.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

#include <numbers>

static constexpr auto deg_to_rad = std::numbers::pi_v<float> / 180.0f;

namespace ot::math
{
	quaternion quaternion::identity() noexcept
	{
		return boost::qvm::identity_quat<float>();
	}

	quaternion quaternion::x_rad_rotation(float angle) noexcept
	{
		return boost::qvm::rotx_quat(angle);
	}

	quaternion quaternion::y_rad_rotation(float angle) noexcept
	{
		return boost::qvm::roty_quat(angle);
	}

	quaternion quaternion::z_rad_rotation(float angle) noexcept
	{
		return boost::qvm::rotz_quat(angle);
	}

	quaternion quaternion::x_deg_rotation(float angle) noexcept
	{
		return x_rad_rotation(angle * deg_to_rad);
	}

	quaternion quaternion::y_deg_rotation(float angle) noexcept
	{
		return y_rad_rotation(angle * deg_to_rad);
	}
	
	quaternion quaternion::z_deg_rotation(float angle) noexcept
	{
		return z_rad_rotation(angle * deg_to_rad);
	}

	quaternion normalized(quaternion const& q) noexcept
	{
		return boost::qvm::normalized(q);
	}

	quaternion invert(quaternion const& q) noexcept
	{
		return boost::qvm::inverse(q);
	}

	quaternion concatenate(quaternion const& lhs, quaternion const& rhs) noexcept
	{
		using boost::qvm::operator*;
		return lhs * rhs;
	}

	quaternion operator*(quaternion const& lhs, float rhs) noexcept
	{
		return boost::qvm::operator*(lhs, rhs);
	}

	vector3f rotate(vector3f v, quaternion q) noexcept
	{
		using boost::qvm::operator*;
		return q * v;
	}

	bool float_eq(quaternion const& lhs, quaternion const& rhs)
	{
		return boost::qvm::cmp(lhs, rhs, [](float lhs, float rhs)
		{
			return float_eq(lhs, rhs);
		});
	}
}
