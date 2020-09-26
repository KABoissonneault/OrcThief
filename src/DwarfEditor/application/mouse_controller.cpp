#include "mouse_controller.h"

#include "application.h"

#include <cassert>

namespace ot::dedit
{
	void mouse_controller::reset() noexcept
	{
		current_state.reset();
		current_action.reset();
	}

	bool mouse_controller::is_in_click_range(int x, int y) const
	{
		mouse_state const& state = *current_state;
		return std::abs(state.start_x - x) <= 3 && std::abs(state.start_y - y) <= 3;
	}

	void mouse_controller::start_frame()
	{
		if (current_action == input::mouse::action_type::click || current_action == input::mouse::action_type::drag_end)
			reset();
	}

	bool mouse_controller::handle_mouse_button_event(SDL_MouseButtonEvent const& e)
	{
		if (e.state == SDL_PRESSED)
		{
			// If we're already pressing a button, just cancel everything
			if (current_state)
			{
				reset();
				return true;
			}

			auto const current_button = input::mouse::button_type_from_sdl(e.button);

			// Don't accept more presses if there's already other buttons pressed
			if (input::mouse::get_buttons() != current_button)
			{
				return true;
			}

			mouse_state& state = current_state.emplace();
			state.button = current_button;
			state.current_x = state.start_x = e.x;
			state.current_y = state.start_y = e.y;

			return true;
		}
		else
		{
			// If we got a release event leaked from another system, just ignore it
			if (!current_state)
			{
				return false;
			}

			mouse_state& state = *current_state;

			// Handling a click
			if (!current_action)
			{
				if (is_in_click_range(e.x, e.y))
				{
					current_action = input::mouse::action_type::click;
				}
				else
				{
					reset();
				}
				return true;
			}
			// Handling a drag end
			else
			{
				assert(current_action == input::mouse::action_type::drag_start || current_action == input::mouse::action_type::drag_update);

				state.current_x = e.x;
				state.current_y = e.y;
				current_action = input::mouse::action_type::drag_end;

				return true;
			}
		}
	}

	bool mouse_controller::handle_mouse_motion_event(SDL_MouseMotionEvent const& e)
	{
		if (!current_state)
			return false;

		mouse_state& state = *current_state;

		// Handle drag start
		if (!current_action)
		{
			if (!is_in_click_range(e.x, e.y))
			{
				current_action = input::mouse::action_type::drag_start;
				return true;
			} 
			else
			{
				return false;
			}
		} 
		// Handle drag update
		else if(current_action == input::mouse::action_type::drag_start || current_action == input::mouse::action_type::drag_update)
		{
			current_action = input::mouse::action_type::drag_update;
			state.current_x = e.x;
			state.current_y = e.y;

			return true;
		}		

		return false;
	}

	std::optional<input::mouse::action> mouse_controller::get_action() const noexcept
	{
		if (!current_action)
			return std::nullopt;

		mouse_state const& state = *current_state;
		return input::mouse::action{ state.button, state.current_x, state.current_y, *current_action };
	}
}
