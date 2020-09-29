#pragma once

#include "base.fwd.h"

#include "map.fwd.h"

namespace ot::dedit::action
{
	class base
	{
	public:
		virtual ~base();

		virtual void apply(map& current_map) = 0;
		virtual void undo(map& current_map) = 0;
	};
}
