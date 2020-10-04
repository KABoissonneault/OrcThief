#include "input.h"

#include "SDL2/macro.h"

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

		void set_relative_mode(bool value)
		{
			OT_SDL_ENSURE(SDL_SetRelativeMouseMode(value ? SDL_TRUE : SDL_FALSE));
		}
	}	

	namespace keyboard
	{
		constexpr auto k_modifiers = KMOD_CTRL | KMOD_SHIFT | KMOD_ALT;

		mod get_modifiers()
		{			
			return mod{ SDL_GetModState() & k_modifiers };
		}

		bool is_pressed(SDL_Scancode key)
		{
			int num_key;
			Uint8 const* const keys = SDL_GetKeyboardState(&num_key);
			assert(key < num_key);
			return keys[key];
		}

		bool is_key_press(SDL_KeyboardEvent const& e, SDL_Keycode key)
		{
			return e.keysym.sym == key
				&& e.state == SDL_PRESSED
				&& e.repeat == 0
				&& (e.keysym.mod & k_modifiers) == 0;
		}

		bool is_key_press(SDL_KeyboardEvent const& e, SDL_Keycode key, mod m)
		{
			return e.keysym.sym == key
				&& e.state == SDL_PRESSED
				&& e.repeat == 0
				&& mod(e.keysym.mod & k_modifiers) == m;
		}

		bool is_key_press(SDL_KeyboardEvent const& e, SDL_Keycode key, mod_group mg)
		{
			return e.keysym.sym == key
				&& e.state == SDL_PRESSED
				&& e.repeat == 0
				&& mod(e.keysym.mod & k_modifiers) == mg;
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
