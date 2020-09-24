#pragma once

#include "core/directive.h"

#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <SDL_keycode.h>

#include <optional>

namespace ot::dedit::input
{
	namespace mouse
	{
		// Button flags
		enum class button_type
		{
			none = 0,
			left = SDL_BUTTON_LMASK,
			middle = SDL_BUTTON_MMASK,
			right = SDL_BUTTON_RMASK,
		};

		[[nodiscard]] inline button_type button_type_from_sdl(int button) 
		{
			switch (button)
			{
			case SDL_BUTTON_LEFT: return button_type::left;
			case SDL_BUTTON_MIDDLE: return button_type::middle;
			case SDL_BUTTON_RIGHT: return button_type::right;
			}
			OT_UNREACHABLE();
		}

		void get_position(int& x, int& y);
		[[nodiscard]] button_type get_buttons();

		enum class action_type
		{
			// An action where a mouse button was pressed then released at the same position (with a small tolerance)
			click,
			// An action where a mouse button is pressed, held, and moved further than the click tolerance
			drag_start,
			// After drag_start, every mouse movement will trigger this action while the button is held
			drag_update,
			// An action when the button is released (or other mouse buttons were pressed) after drag_start
			drag_end,
		};

		struct action
		{
			button_type button;
			int x, y;
			action_type action_type;
		};

		[[nodiscard]] inline bool is_left_click(action const& a) noexcept { return a.action_type == action_type::click && a.button == button_type::left; }
		[[nodiscard]] inline bool is_right_click(action const& a) noexcept { return a.action_type == action_type::click && a.button == button_type::right; }
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

		[[nodiscard]] mod get_modifiers();
		[[nodiscard]] bool is_pressed(SDL_Scancode key);
	}

	// Structure containing the user input that may have occured in a single frame
	struct frame_input
	{
		std::optional<mouse::action> mouse_action;

		[[nodiscard]] bool consume_left_click() noexcept;
		[[nodiscard]] bool consume_right_click() noexcept;
	};
}
