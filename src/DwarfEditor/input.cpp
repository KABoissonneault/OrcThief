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

		button get_buttons()
		{
			return static_cast<button>(SDL_GetMouseState(nullptr, nullptr));
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
}