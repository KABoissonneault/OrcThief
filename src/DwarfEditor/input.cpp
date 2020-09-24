#include "input.h"

#include <SDL_keyboard.h>

#include <cassert>

namespace ot::dedit::input
{
	namespace mouse
	{
		void get_position(int& x, int& y)
		{
			SDL_GetMouseState(&x, &y);
		}

		button_type get_buttons()
		{
			return static_cast<button_type>(SDL_GetMouseState(nullptr, nullptr));
		}
	}	

	namespace keyboard
	{
		mod get_modifiers()
		{
			constexpr auto k_modifiers = KMOD_CTRL | KMOD_SHIFT | KMOD_ALT;
			return mod{ SDL_GetModState() & k_modifiers };
		}

		bool is_pressed(SDL_Scancode key)
		{
			int num_key;
			Uint8 const* const keys = SDL_GetKeyboardState(&num_key);
			assert(key < num_key);
			return keys[key];
		}
	}

	bool frame_input::consume_left_click() noexcept
	{
		if (mouse_action && is_left_click(*mouse_action))
		{
			mouse_action.reset();
			return true;
		}

		return false;
	}

	bool frame_input::consume_right_click() noexcept
	{
		if (mouse_action && is_right_click(*mouse_action))
		{
			mouse_action.reset();
			return true;
		}

		return false;
	}
}
