#pragma once

#include "math/quaternion.h"
#include "math/boost/detail/common.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/quat_traits.hpp>
#include <boost/qvm/quat_traits_defaults.hpp>
#include <boost/qvm/deduce_vec.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

template<>
struct boost::qvm::is_quat<ot::math::quaternion>
{
	static bool const value = true;
};

template<>
struct boost::qvm::quat_traits<ot::math::quaternion> : boost::qvm::quat_traits_defaults<ot::math::quaternion, float>
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
struct boost::qvm::deduce_vec2<ot::math::quaternion, ot::math::vector3f, 3>
{
	using type = ot::math::vector3f;
};
