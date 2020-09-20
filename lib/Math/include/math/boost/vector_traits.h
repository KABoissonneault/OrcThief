#pragma once

#include "math/vector3.h"
#include "math/boost/detail/common.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/vec_traits.hpp>
#include <boost/qvm/vec_traits_defaults.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

template<>
struct boost::qvm::is_vec<ot::math::point3f>
{
	static bool const value = true;
};

template<>
struct boost::qvm::vec_traits<ot::math::point3f> : boost::qvm::vec_traits_defaults<ot::math::point3f, float, 3>
{
	template<int I>
	static inline float& write_element(ot::math::point3f& v)
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
struct boost::qvm::is_vec<ot::math::vector3f>
{
	static bool const value = true;
};

template<>
struct boost::qvm::vec_traits<ot::math::vector3f> : boost::qvm::vec_traits_defaults<ot::math::vector3f, float, 3>
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
