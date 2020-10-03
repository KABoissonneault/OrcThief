#pragma once

#include "math/transform_matrix.h"
#include "math/boost/transform_matrix_traits.h"

#include "Ogre/Matrix4.h"
#include "Ogre/boost/Matrix4Traits.h"

#include <boost/qvm/mat_operations4.hpp>

namespace ot::egfx
{
	[[nodiscard]] inline math::transform_matrix to_math_matrix(Ogre::Matrix4 const& m) noexcept
	{
		return boost::qvm::convert_to<math::transform_matrix>(m);
	}

	[[nodiscard]] inline Ogre::Matrix4 to_ogre_matrix(math::transform_matrix const& m) noexcept
	{
		return boost::qvm::convert_to<Ogre::Matrix4>(m);
	}
}