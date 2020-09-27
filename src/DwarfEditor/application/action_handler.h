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
		std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> current_brush;
		std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> previous_brush;
		std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> redo_brush;

	public:
		virtual void push_brush_action(uptr<action::brush_base, fwd_delete<action::brush_base>> action);
		
		bool handle_keyboard_event(SDL_KeyboardEvent const& key, map& current_map);
		
		void apply_actions(map& current_map);
		void undo_latest(map& current_map);
		void redo_latest(map& current_map);

		[[nodiscard]] bool has_undo() const noexcept { return !previous_brush.empty(); }
		[[nodiscard]] bool has_redo() const noexcept { return !redo_brush.empty(); }

		void clear();
	};
}
