#pragma once

#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <SDL_keycode.h>

namespace ot::dedit::input
{
	namespace mouse
	{
		enum class button
		{
			left = SDL_BUTTON_LMASK,
			middle = SDL_BUTTON_MMASK,
			right = SDL_BUTTON_RMASK,
		};

		void get_position(int& x, int& y);
		button get_buttons();
	}

	namespace keyboard
	{
		enum class mod
		{
			lshift = KMOD_LSHIFT,
			rshift = KMOD_RSHIFT,
			lctrl = KMOD_LCTRL,
			rctrl = KMOD_RCTRL,
			lalt = KMOD_LALT,
			ralt = KMOD_RALT,
		};

		enum class mod_group
		{
			shift = KMOD_SHIFT,
			ctrl = KMOD_CTRL,
			alt = KMOD_ALT,
		};

		[[nodiscard]] inline constexpr bool operator==(mod lhs, mod_group rhs) noexcept
		{
			return (static_cast<int>(lhs) & static_cast<int>(rhs)) && !(static_cast<int>(lhs) & ~static_cast<int>(rhs));
		}

		mod get_modifiers();
		bool is_pressed(SDL_Scancode key);
	}
}