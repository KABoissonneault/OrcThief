#pragma once

#include "imgui/matrix.h"

namespace ot::dedit::imgui
{
	matrix make_perspective_projection(float fov_y, float aspect_ratio, float z_near, float z_far);
}
