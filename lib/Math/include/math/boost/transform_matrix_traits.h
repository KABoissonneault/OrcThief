#pragma once

#include "math/transform_matrix.h"
#include "math/boost/detail/common.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_traits.hpp>
#include <boost/qvm/mat_traits_defaults.hpp>
#include <boost/qvm/deduce_vec.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

template<>
struct boost::qvm::is_mat<ot::math::rotation_matrix>
{
	static bool const value = true;
};

template<>
struct boost::qvm::mat_traits<ot::math::rotation_matrix> : boost::qvm::mat_traits_defaults<ot::math::rotation_matrix, float, 3, 3>
{
	template<int R, int C>
	static inline float& write_element(ot::math::rotation_matrix& m)
	{
		static_assert(R < 3 && C < 3, "Out-of-bounds");
		return m.elements[R * 3 + C];
	}

	static inline float& write_element_idx(int r, int c, ot::math::rotation_matrix& m)
	{
		assert(r < 3 && c < 3);
		return m.elements[r * 3 + c];
	}
};

template<>
struct boost::qvm::is_mat<ot::math::transform_matrix>
{
	static bool const value = true;
};

template<>
struct boost::qvm::mat_traits<ot::math::transform_matrix> : boost::qvm::mat_traits_defaults<ot::math::transform_matrix, float, 4, 4>
{
	template<int R, int C>
	static inline float& write_element(ot::math::transform_matrix& m)
	{
		static_assert(R < 4 && C < 4, "Out-of-bounds");
		return m.elements[R * 4 + C];
	}

	static inline float& write_element_idx(int r, int c, ot::math::transform_matrix& m)
	{
		assert(r < 4 && c < 4);
		return m.elements[r * 4 + c];
	}
};

template<>
struct boost::qvm::deduce_vec2<ot::math::rotation_matrix, ot::math::vector3f, 3>
{
	using type = ot::math::vector3f;
};

template<>
struct boost::qvm::deduce_vec2<ot::math::transform_matrix, ot::math::point3f, 3>
{
	using type = ot::math::point3f;
};
