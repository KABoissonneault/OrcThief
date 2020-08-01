#pragma once

#include "math/vector3.h"

namespace ot::graphics
{
	class camera;

	void set_position(camera& c, math::vector3d position);
	void look_at(camera& c, math::vector3d position);
}