#include "math/quaternion.h"

#include "core/compiler.h"

#if OT_COMPILER_MSVC
#define OT_BOOST_INCLUDE_BEGIN \
	__pragma(warning(push)) \
	__pragma(warning(disable:4100)) /* unreferenced formal parameter */ 
#define OT_BOOST_INCLUDE_END \
	__pragma(warning(pop))
#else
#define OT_BOOST_INCLUDE_BEGIN
#define OT_BOOST_INCLUDE_END
#endif

OT_BOOST_INCLUDE_BEGIN
#include <boost/qvm/quat_traits.hpp>
#include <boost/qvm/quat_traits_defaults.hpp>
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/quat_vec_operations.hpp>

#include <boost/qvm/vec_traits.hpp>
#include <boost/qvm/vec_traits_defaults.hpp>

#include <boost/qvm/quat_vec_operations.hpp>
OT_BOOST_INCLUDE_END

#include <numbers>

static constexpr auto deg_to_rad = std::numbers::pi_v<float> / 180.0f;

namespace boost::qvm
{
	template<>
	struct is_quat<ot::math::quaternion>
	{
		static bool const value = true;
	};

	template<>
	struct quat_traits<ot::math::quaternion> : quat_traits_defaults<ot::math::quaternion, float>
	{
		template<int I>
		static inline float& write_element(ot::math::quaternion& q)
		{
			if constexpr (I == 0)
			{
				return q.w;
			}
			else if constexpr (I == 1)
			{
				return q.x;
			}
			else if constexpr (I == 2)
			{
				return q.y;
			}
			else if constexpr (I == 3)
			{
				return q.z;
			}
		}
	};

	template<>
	struct is_vec<ot::math::vector3f>
	{
		static bool const value = true;
	};

	template<>
	struct vec_traits<ot::math::vector3f> : vec_traits_defaults<ot::math::vector3f, float, 3>
	{
		template<int I>
		static inline float& write_element(ot::math::vector3f& v)
		{
			if constexpr (I == 0)
			{
				return v.x;
			} 
			else if constexpr (I == 1)
			{
				return v.y;
			} 
			else if constexpr (I == 2)
			{
				return v.z;
			} 
		}
	};

	template<>
	struct deduce_vec2<ot::math::quaternion, ot::math::vector3f, 3>
	{
		using type = ot::math::vector3f;
	};
}

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
