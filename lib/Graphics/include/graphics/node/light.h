#pragma once

#include "graphics/node/light.fwd.h"
#include "graphics/node/object.h"

namespace ot::graphics::node
{
	class directional_light : public object 
	{ 
	};

	[[nodiscard]] directional_light create_directional_light(object& parent);
}
