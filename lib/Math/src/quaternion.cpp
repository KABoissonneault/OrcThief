#include "math/quaternion.h"

#include <boost/qvm/quat_traits.hpp>
#include <boost/qvm/quat_traits_defaults.hpp>
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/quat_vec_operations.hpp>

#include <boost/qvm/vec_traits.hpp>
#include <boost/qvm/vec_traits_defaults.hpp>

#include <boost/qvm/quat_vec_operations.hpp>

namespace boost::qvm
{
	template<>
	struct is_quat<ot::math::quaternion>
	{
		static bool const value = true;
	};

	template<>
	struct quat_traits<ot::math::quaternion> : quat_traits_defaults<ot::math::quaternion, double>
	{
		template<int I>
		static inline double& write_element(ot::math::quaternion& q)
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
	struct is_vec<ot::math::vector3d>
	{
		static bool const value = true;
	};

	template<>
	struct vec_traits<ot::math::vector3d> : vec_traits_defaults<ot::math::vector3d, double, 3>
	{
		template<int I>
		static inline double& write_element(ot::math::vector3d& v)
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
	struct deduce_vec2<ot::math::quaternion, ot::math::vector3d, 3>
	{
		using type = ot::math::vector3d;
	};
}

namespace ot::math
{
	quaternion quaternion::make_rotx(double angle)
	{
		return boost::qvm::rotx_quat(angle);
	}

	quaternion quaternion::make_roty(double angle)
	{
		return boost::qvm::roty_quat(angle);
	}

	quaternion quaternion::make_rotz(double angle)
	{
		return boost::qvm::rotz_quat(angle);
	}

	vector3d rotate(vector3d v, quaternion q)
	{
		using boost::qvm::operator*;
		return q * v;
	}
}
