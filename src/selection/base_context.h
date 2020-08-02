#pragma once

#include "core/uptr.h"

#include "selection/context.h"
#include "map.h"

#include "graphics/scene.h"
#include "graphics/window.h"

namespace ot::selection
{
	// The context at the root of the application. Handles object selection and global events
	class base_context : public context
	{
		map* current_map;
		graphics::scene const* current_scene;
		graphics::window const* main_window;
		uptr<context> next_context;

	public:
		base_context(map& current_map, graphics::scene const& current_scene, graphics::window const& main_window) noexcept
			: current_map(&current_map)
			, current_scene(&current_scene)
			, main_window(&main_window)
		{

		}

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse) override;
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse) override;
	};
}
