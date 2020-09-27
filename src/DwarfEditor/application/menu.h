#pragma once

#include <SDL_events.h>

namespace ot::dedit
{
	class application;

	template<typename Application>
	class menu
	{
		using derived = Application;

		bool draw_console_window = false;

	public:
		bool handle_keyboard_event(SDL_KeyboardEvent const& e);

		void update();
	};

	extern template class menu<application>;
}
