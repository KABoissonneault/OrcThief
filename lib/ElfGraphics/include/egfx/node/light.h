#pragma once

#include "egfx/node/light.fwd.h"
#include "egfx/node/object.h"

namespace ot::egfx::node
{
	class directional_light : public object 
	{ 
		friend directional_light create_directional_light(object_ref parent);
	};

	[[nodiscard]] directional_light create_directional_light(object_ref parent);
}
