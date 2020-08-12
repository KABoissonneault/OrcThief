#pragma once

#include "core/uptr.h"
#include "math/unit/time.h"

#include "graphics/node/manual.h"

#include <SDL_events.h>

#include <string>

namespace ot::selection
{
	class context
	{
	public:
		virtual ~context() = 0;

		virtual void update(math::seconds dt) { (void)dt; }
		virtual void render(graphics::node::manual& m) { (void)m; }

		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key) { (void)key; return false; }
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse) { (void)mouse; return false; }
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse) { (void)mouse; return false; }

		virtual void get_debug_string(std::string& debug) const { (void)debug; }
	};

	class composite_context : public context
	{
	protected:
		uptr<context> next_context;

	public:
		virtual void update(math::seconds dt) override;
		virtual void render(graphics::node::manual& m) override;

		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse) override;
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse) override;

		virtual void get_debug_string(std::string& debug) const override;
	};
}