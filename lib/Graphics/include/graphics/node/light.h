#pragma once

#include "graphics/node/light.fwd.h"
#include "graphics/node/object.h"

namespace ot::graphics::node
{
	class directional_light : public object 
	{ 
		friend directional_light create_directional_light(object_ref parent);
	};

	[[nodiscard]] directional_light create_directional_light(object_ref parent);
}
