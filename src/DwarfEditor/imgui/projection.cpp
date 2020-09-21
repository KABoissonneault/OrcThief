#include "imgui/projection.h"

#include "imgui/boost/common.h"
#include "imgui/boost/matrix_traits.h"

OT_IMGUI_DETAIL_BOOST_INCLUDE_BEGIN
#include <boost/qvm/mat_operations.hpp>
OT_IMGUI_DETAIL_BOOST_INCLUDE_END

namespace ot::dedit::imgui
{
	matrix make_perspective_projection(float fov_y, float aspect_ratio, float z_near, float z_far)
	{
		return boost::qvm::perspective_rh(fov_y, aspect_ratio, z_near, z_far);
	}
}
