#pragma once

#include "math/vector3.h"

#include "math/ray.h"

namespace ot::egfx
{
	class camera;

	void set_position(camera& c, math::point3d position);
	[[nodiscard]] math::point3d get_position(camera const& c);
	void look_at(camera& c, math::point3d position); 
	// Projects a raycast from the viewport of the main camera to the world
	// 
	//   viewport_x: distance from the left of the screen in normalized coordinates [0, 1]
	//   viewport_y: distance from the top of the screen in normalizes coordinates [0, 1]
	//
	// Returns: ray from viewport position toward the world
	[[nodiscard]] math::ray get_world_ray_from_viewport(camera const& c, double viewport_x, double viewport_y);
}