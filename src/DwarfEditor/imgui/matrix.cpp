#include "imgui/matrix.h"
#include "imgui/boost/matrix_traits.h"

#include "math/transform_matrix.h"
#include "math/boost/transform_matrix_traits.h"

OT_IMGUI_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/map_mat_mat.hpp>
OT_IMGUI_DETAIL_BOOST_INCLUDE_END

namespace ot::dedit::imgui
{
	matrix matrix::identity() noexcept
	{
		return boost::qvm::identity_mat<float, 4>();
	}
}

namespace ot::math
{
	dedit::imgui::matrix to_imgui(transform_matrix const& m)
	{
		return boost::qvm::convert_to<dedit::imgui::matrix>(m);
	}
}
