#include "selection/context.h"

namespace ot::dedit::selection
{
	context::~context() = default;

	void composite_context::update(egfx::node::manual& m, action::accumulator& acc)
	{
		if (next_context != nullptr)
			next_context->update(m, acc);
	}
	
	bool composite_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (next_context != nullptr)
			return next_context->handle_keyboard_event(key, acc);
		return false;
	}

	bool composite_context::handle_mouse_button_event(SDL_MouseButtonEvent const& mouse, action::accumulator& acc)
	{
		if (next_context != nullptr)
			return next_context->handle_mouse_button_event(mouse, acc);
		return false;
	}

	bool composite_context::handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse, action::accumulator& acc)
	{
		if (next_context != nullptr)
			return next_context->handle_mouse_motion_event(mouse, acc);
		return false;
	}

	void composite_context::get_debug_string(std::string& debug) const
	{
		if (next_context != nullptr)
			next_context->get_debug_string(debug);
	}
}