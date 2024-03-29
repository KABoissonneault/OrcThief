#pragma once

#include "core/stdint.h"

namespace ot::dedit
{
	enum class entity_id : uint64_t { root = 0 };

	enum class entity_type
	{
		root,
		brush,
		light,
	};

	class map_entity;
	class root_entity;
	class brush_entity;
	class light_entity;
	class map;
}
