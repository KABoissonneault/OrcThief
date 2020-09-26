#pragma once

#include "input.h"

#include <SDL_events.h>

#include <optional>

namespace ot::dedit
{
	class application;

	template<typename Application>
	class mouse_controller
	{
		using derived = Application;

		struct mouse_state
		{
			input::mouse::button_type button;
			int start_x, start_y;
			int current_x, current_y;
		};

		std::optional<mouse_state> current_state;
		std::optional<input::mouse::action_type> current_action;

		void reset() noexcept;
		bool is_in_click_range(int x, int y) const;

	public:
		void start_frame();

		bool handle_mouse_button_event(SDL_MouseButtonEvent const& e);
		bool handle_mouse_motion_event(SDL_MouseMotionEvent const& e);

		std::optional<input::mouse::action> get_action() const noexcept;
	};

	extern template class mouse_controller<application>;
}
