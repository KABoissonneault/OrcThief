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
		struct action_data
		{
			fwd_uptr<action::base> action;
			int id;
		};

		std::vector<action_data> current_actions;
		std::vector<action_data> applied_actions;
		std::vector<action_data> undone_actions;

		int next_id = 0;

	public:
		virtual void push_action(fwd_uptr<action::base> action) override;
		
		bool handle_keyboard_event(SDL_KeyboardEvent const& key, map& current_map);
		
		void apply_actions(map& current_map);
		void undo_latest(map& current_map);
		void redo_latest(map& current_map);

		// IDs representing a monotonically increasing counter of applied actions
		// This is used by ex: the map handler to tell if actions were applied since the map was last saved
		[[nodiscard]] int get_last_action() const noexcept;

		[[nodiscard]] bool has_undo() const noexcept { return !applied_actions.empty(); }
		[[nodiscard]] bool has_redo() const noexcept { return !undone_actions.empty(); }

		void clear();
	};
}
