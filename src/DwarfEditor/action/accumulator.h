#pragma once

#include "action/base.fwd.h"

#include "core/uptr.h"

#include <type_traits>

namespace ot::dedit::action
{
	class accumulator
	{
	public:
		virtual void push_action(fwd_uptr<base> p) = 0;

		template<typename Action, typename... Args>
		void emplace_action(Args&&... args)
		{
			push_action(fwd_uptr<base>(new Action(std::forward<Args>(args)...)));
		}
	};
}