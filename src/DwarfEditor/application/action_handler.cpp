#include "application/action_handler.h"

#include "action/brush.h"

#include "input.h"
#include "console.h"

#include <cassert>

#include <SDL_keycode.h>

namespace ot::dedit
{
	void action_handler::push_action(fwd_uptr<action::base> action)
	{
		current_actions.push_back({ std::move(action), ++next_id });
	}

	bool action_handler::handle_keyboard_event(SDL_KeyboardEvent const& key, map& current_map)
	{
		auto const modifiers = input::keyboard::get_modifiers();

		if (is_key_press(key, SDLK_z, input::keyboard::mod_group::ctrl))
		{
			if (!has_undo())
				console::error("Could not undo: no actions in stack");
			else
				undo_latest(current_map);
			return true;
		}

		if (is_key_press(key, SDLK_y, input::keyboard::mod_group::ctrl))
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
		if (current_actions.empty())
			return;

		for (auto& data : current_actions)
		{
			data.action->apply(current_map);
		}

		applied_actions.insert(applied_actions.end(), std::make_move_iterator(current_actions.begin()), std::make_move_iterator(current_actions.end()));
		current_actions.clear();
		undone_actions.clear();
	}

	void action_handler::clear()
	{
		current_actions.clear();
		applied_actions.clear();
		undone_actions.clear();
	}

	void action_handler::redo_latest(map& current_map)
	{
		assert(has_redo());

		auto& data = undone_actions.back();
		data.action->apply(current_map);
		applied_actions.push_back(std::move(data));
		undone_actions.pop_back();
	}

	void action_handler::undo_latest(map& current_map)
	{
		assert(has_undo());

		auto& data = applied_actions.back();
		data.action->undo(current_map);
		undone_actions.push_back(std::move(data));
		applied_actions.pop_back();
	}

	int action_handler::get_last_action() const noexcept
	{
		if (applied_actions.empty())
		{
			return 0;
		}
		else
		{
			return applied_actions.back().id;
		}
	}
}
