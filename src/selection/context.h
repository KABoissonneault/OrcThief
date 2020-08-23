#pragma once

#include "core/uptr.h"
#include "math/unit/time.h"

#include "graphics/node/manual.h"

#include "action/accumulator.h"

#include "map.h"

#include <SDL_events.h>

#include <string>

namespace ot::selection
{
	class context
	{
	public:
		virtual ~context() = 0;

		virtual void update() { }
		virtual void render(graphics::node::manual& m) { (void)m; }

		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) { (void)key, (void) acc; return false; }
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse, action::accumulator& acc) { (void)mouse, (void) acc; return false; }
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse, action::accumulator& acc) { (void)mouse, (void) acc; return false; }

		virtual bool is_selected(entity_id id) const noexcept { (void)id; return false; }

		virtual void get_debug_string(std::string& debug) const { (void)debug; }
	};

	class composite_context : public context
	{
	protected:
		uptr<context> next_context;

	public:
		virtual void update() override;
		virtual void render(graphics::node::manual& m) override;

		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse, action::accumulator& acc) override;
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse, action::accumulator& acc) override;


		virtual bool is_selected(entity_id id) const noexcept { return next_context != nullptr ? next_context->is_selected(id) : false; }

		virtual void get_debug_string(std::string& debug) const override;
	};
}