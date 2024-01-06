#pragma once

#include "math/ray.h"
#include "egfx/window.h"
#include "egfx/object/camera.fwd.h"

namespace ot::dedit::selection
{
	math::ray get_mouse_ray(egfx::window const& window, egfx::camera_cref camera);
}
