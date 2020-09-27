#include "application/action_handler.h"

#include "action/brush.h"

#include "input.h"
#include "console.h"

#include <cassert>

namespace ot::dedit
{
	void action_handler::push_brush_action(uptr<action::brush_base, fwd_delete<action::brush_base>> action)
	{
		current_brush.push_back(std::move(action));
	}

	bool action_handler::handle_keyboard_event(SDL_KeyboardEvent const& key, map& current_map)
	{
		auto const modifiers = input::keyboard::get_modifiers();

		if (key.keysym.scancode == SDL_SCANCODE_Z
			&& key.state == SDL_PRESSED
			&& key.repeat == 0
			&& modifiers == input::keyboard::mod_group::ctrl
			)
		{
			if (!has_undo())
				console::error("Could not undo: no actions in stack");
			else
				undo_latest(current_map);
			return true;
		}

		if (key.keysym.scancode == SDL_SCANCODE_Y
			&& key.state == SDL_PRESSED
			&& key.repeat == 0
			&& modifiers == input::keyboard::mod_group::ctrl
			)
		{
			if (!has_redo())
				console::error("Could not redo: no actions in stack");
			else
				redo_latest(current_map);

			return true;
		}

		return false;
	}

	void action_handler::apply_actions(map& current_map)
	{
		if (current_brush.empty())
			return;

		for (auto& action : current_brush)
		{
			action->apply(current_map);
		}

		previous_brush.insert(previous_brush.end(), std::make_move_iterator(current_brush.begin()), std::make_move_iterator(current_brush.end()));
		current_brush.clear();
		redo_brush.clear();
	}

	void action_handler::clear()
	{
		current_brush.clear();
		previous_brush.clear();
		redo_brush.clear();
	}

	void action_handler::redo_latest(map& current_map)
	{
		assert(has_redo());

		auto& brush = redo_brush.back();
		brush->apply(current_map);
		previous_brush.push_back(std::move(brush));
		redo_brush.pop_back();
	}

	void action_handler::undo_latest(map& current_map)
	{
		assert(has_undo());

		auto& brush = previous_brush.back();
		brush->undo(current_map);
		redo_brush.push_back(std::move(brush));
		previous_brush.pop_back();
	}
}
