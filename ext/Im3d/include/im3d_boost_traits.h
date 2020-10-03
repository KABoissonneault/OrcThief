#pragma once

#include <boost/qvm/mat_traits.hpp>
#include <boost/qvm/mat_traits_defaults.hpp>

#include "im3d.h"

template<>
struct boost::qvm::is_mat<Im3d::Mat3>
{
	static bool const value = true;
};

template<>
struct boost::qvm::is_mat<Im3d::Mat4>
{
	static bool const value = true;
};

template<>
struct boost::qvm::mat_traits<Im3d::Mat3> : boost::qvm::mat_traits_defaults<Im3d::Mat3, float, 3, 3>
{
	template<int R, int C>
	static inline float& write_element(Im3d::Mat3& m)
	{
		static_assert(R < 3 && C < 3, "Out-of-bounds");
		return m.m[C * 3 + R];
	}

	static inline float& write_element_idx(int r, int c, Im3d::Mat3& m)
	{
		assert(r < 3 && c < 3);
		return m.m[c * 3 + r];
	}
};

template<>
struct boost::qvm::mat_traits<Im3d::Mat4> : boost::qvm::mat_traits_defaults<Im3d::Mat4, float, 4, 4>
{
	template<int R, int C>
	static inline float& write_element(Im3d::Mat4& m)
	{
		static_assert(R < 4 && C < 4, "Out-of-bounds");
		return m.m[C * 4 + R];
	}

	static inline float& write_element_idx(int r, int c, Im3d::Mat4& m)
	{
		assert(r < 4 && c < 4);
		return m.m[c * 4 + r];
	}
};
