#pragma once

#include "core/stdint.h"

namespace ot::egfx
{
	class object_cref;
	class object_ref;

	class object_iterator;
	class object_range;
	class object_const_iterator;
	class object_const_range;
		
	enum class object_id : uint32_t {};
	enum class object_type : uint8_t
	{
		item,
		camera,
		light,
	};
}