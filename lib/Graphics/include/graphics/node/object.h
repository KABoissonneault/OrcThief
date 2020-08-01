#pragma once

#include "graphics/node/object.fwd.h"

#include "math/vector3.h"

namespace ot::graphics::node
{
	void set_position(object& n, math::vector3d position);
	void set_direction(object& n, math::vector3d direction);
	void rotate_around(object& n, math::vector3d axis, double rad);

	void attach_child(object& n, object& child);
}
