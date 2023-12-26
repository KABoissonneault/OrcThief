#pragma once

#include "egfx/mesh_definition.fwd.h"

#include <SDL_events.h>

#include <memory>

namespace ot::dedit
{
	class application;

	template<typename Application>
	class menu
	{
		using derived = Application;

		bool draw_console_window = false;
		bool draw_about_window = false;
		bool draw_imgui_demo = false;

		size_t last_error_count = 0;

		void draw_main_menu();
		void draw_main_status();
		
	public:
		menu();

		bool handle_keyboard_event(SDL_KeyboardEvent const& e);

		void update();
	};

	extern template class menu<application>;
}
