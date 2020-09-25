#pragma once

#include "math/transform_matrix.h"
#include "math/boost/detail/common.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

namespace ot::math::ops
{
	template<typename TransformMatrix>
	math::scales get_scale(TransformMatrix const& m)
	{
		auto const& rotation_mat = boost::qvm::del_row_col<3, 3>(m);
		return {
			boost::qvm::mag(boost::qvm::col<0>(rotation_mat))
			, boost::qvm::mag(boost::qvm::col<1>(rotation_mat))
			, boost::qvm::mag(boost::qvm::col<2>(rotation_mat))
		};
	}

	template<typename TransformMatrix>
	math::rotation_matrix get_rotation(TransformMatrix const& m)
	{
		using boost::qvm::operator*;

		scales const scale = get_scale(m);
		scales const invert_scale{ 1.f / scale.x, 1.f / scale.y, 1.f / scale.z };

		return boost::qvm::del_row_col<3, 3>(m) * boost::qvm::diag_mat(invert_scale);
	}
}
