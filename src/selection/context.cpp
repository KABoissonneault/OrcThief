#include "selection/context.h"

namespace ot::selection
{
	context::~context() = default;

	void composite_context::update(math::seconds dt)
	{
		if (next_context != nullptr)
			next_context->update(dt);
	}
	
	void composite_context::render(graphics::node::manual& m)
	{
		if (next_context != nullptr)
			next_context->render(m);
	}

	bool composite_context::handle_keyboard_event(SDL_KeyboardEvent const& key)
	{
		if (next_context != nullptr)
			return next_context->handle_keyboard_event(key);
		return false;
	}

	bool composite_context::handle_mouse_button_event(SDL_MouseButtonEvent const& mouse)
	{
		if (next_context != nullptr)
			return next_context->handle_mouse_button_event(mouse);
		return false;
	}

	bool composite_context::handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse)
	{
		if (next_context != nullptr)
			return next_context->handle_mouse_motion_event(mouse);
		return false;
	}

	void composite_context::get_debug_string(std::string& debug) const
	{
		if (next_context != nullptr)
			next_context->get_debug_string(debug);
	}
}