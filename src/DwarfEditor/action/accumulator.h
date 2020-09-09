#pragma once

#include "action/brush.fwd.h"

#include "core/uptr.h"

#include <type_traits>

namespace ot::dedit::action
{
	class accumulator
	{
	public:
		virtual void push_brush_action(fwd_uptr<brush_base> p) = 0;

		template<typename Action, typename... Args>
		void emplace_brush_action(Args&&... args)
		{
			push_brush_action(fwd_uptr<brush_base>(new Action(std::forward<Args>(args)...)));
		}
	};
}