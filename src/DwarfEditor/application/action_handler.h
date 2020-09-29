#pragma once

#include "action/accumulator.h"

#include "map.h"

#include "core/uptr.h"

#include <vector>
#include <SDL_events.h>

namespace ot::dedit
{
	class action_handler : public action::accumulator
	{
		std::vector<fwd_uptr<action::base>> current_actions;
		std::vector<fwd_uptr<action::base>> previous_actions;
		std::vector<fwd_uptr<action::base>> redo_actions;

	public:
		virtual void push_action(fwd_uptr<action::base> action) override;
		
		bool handle_keyboard_event(SDL_KeyboardEvent const& key, map& current_map);
		
		void apply_actions(map& current_map);
		void undo_latest(map& current_map);
		void redo_latest(map& current_map);

		[[nodiscard]] bool has_undo() const noexcept { return !previous_actions.empty(); }
		[[nodiscard]] bool has_redo() const noexcept { return !redo_actions.empty(); }

		void clear();
	};
}
