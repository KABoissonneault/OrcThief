#pragma once

#include "core/stdint.h"

#include "object.fwd.h"

namespace ot::egfx
{
	class light_ref;
	class light_cref;

	enum class light_type : uint8_t
	{
		directional,
		point,
		spotlight,
	};
}
