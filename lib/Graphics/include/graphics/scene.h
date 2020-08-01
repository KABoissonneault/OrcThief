#pragma once

#include "graphics/scene.fwd.h"
#include "graphics/node/object.fwd.h"

namespace ot::graphics
{
	class camera;

	[[nodiscard]] camera& get_camera(scene& scene);
	[[nodiscard]] node::object& get_root_node(scene& scene);
}
