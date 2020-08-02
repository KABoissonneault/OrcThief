#pragma once

#include <SDL_events.h>

namespace ot::selection
{
	class context
	{
	public:
		virtual ~context() = 0;

		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key) = 0;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse) = 0;
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse) = 0;
	};
}