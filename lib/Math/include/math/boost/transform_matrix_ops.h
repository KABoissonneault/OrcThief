#pragma once

#include "math/transform_matrix.h"
#include "math/boost/detail/common.h"

OT_MATH_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat_access.hpp>
OT_MATH_DETAIL_BOOST_INCLUDE_END

#include <cmath>

namespace ot::math::ops
{
	template<typename TransformMatrix>
	scales get_scale(TransformMatrix const& m)
	{
		auto const& rotation_mat = boost::qvm::del_row_col<3, 3>(m);
		return {
			boost::qvm::mag(boost::qvm::col<0>(rotation_mat))
			, boost::qvm::mag(boost::qvm::col<1>(rotation_mat))
			, boost::qvm::mag(boost::qvm::col<2>(rotation_mat))
		};
	}

	template<typename TransformMatrix>
	rotation_matrix get_rotation(TransformMatrix const& m, scales scale)
	{
		using boost::qvm::operator*;
		scales const invert_scale{ 1.f / scale.x, 1.f / scale.y, 1.f / scale.z };

		return boost::qvm::del_row_col<3, 3>(m) * boost::qvm::diag_mat(invert_scale);
	}

	template<typename TransformMatrix>
	rotation_matrix get_rotation(TransformMatrix const& m)
	{
		return get_rotation(m, get_scale(m));
	}

	template<typename RotationMatrix>
	float get_rotx(RotationMatrix const& m)
	{
		using namespace boost::qvm;
		return std::atan2(A21(m), A22(m));
	}

	template<typename RotationMatrix>
	float get_roty(RotationMatrix const& m)
	{
		using namespace boost::qvm;
		return std::asin(-A20(m));
	}

	template<typename RotationMatrix>
	float get_rotz(RotationMatrix const& m)
	{
		using namespace boost::qvm;
		return std::atan2(A10(m), A00(m));
	}
}
