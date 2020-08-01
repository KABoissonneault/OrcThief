#pragma once

#include "graphics/scene.fwd.h"
#include "graphics/node/light.fwd.h"
#include "graphics/node/object.h"

namespace ot::graphics
{
	namespace node
	{
		[[nodiscard]] directional_light create_directional_light(scene& scene, object& parent);
	}
}
