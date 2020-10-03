#pragma once

#include <boost/qvm/mat_traits.hpp>
#include <boost/qvm/mat_traits_defaults.hpp>

#include "Ogre/Matrix4.h"

template<>
struct boost::qvm::is_mat<Ogre::Matrix4>
{
	static bool const value = true;
};

template<>
struct boost::qvm::mat_traits<Ogre::Matrix4> : boost::qvm::mat_traits_defaults<Ogre::Matrix4, float, 4, 4>
{
	template<int R, int C>
	static inline float& write_element(Ogre::Matrix4& m)
	{
		static_assert(R < 4 && C < 4, "Out-of-bounds");
		return m[R][C];
	}

	static inline float& write_element_idx(int r, int c, Ogre::Matrix4& m)
	{
		assert(r < 4 && c < 4);
		return m[r][c];
	}
};
