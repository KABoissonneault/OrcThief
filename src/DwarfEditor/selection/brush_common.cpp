#include "brush_common.h"

#include "input.h"
#include "egfx/object/camera.h"

namespace ot::dedit::selection
{
	math::ray get_mouse_ray(egfx::window const& window, egfx::object::camera_cref camera)
	{
		int mouse_x, mouse_y;
		input::mouse::get_position(mouse_x, mouse_y);

		float const viewport_x = static_cast<float>(mouse_x) / get_width(window);
		float const viewport_y = static_cast<float>(mouse_y) / get_height(window);
		return camera.get_world_ray(viewport_x, viewport_y);
	}
}