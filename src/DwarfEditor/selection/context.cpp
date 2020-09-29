#include "selection/context.h"

namespace ot::dedit::selection
{
	context::~context() = default;

	void composite_context::start_frame()
	{
		if (next_context != nullptr)
			next_context->start_frame();
	}

	void composite_context::update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input)
	{
		if (next_context != nullptr)
			next_context->update(m, acc, input);
	}
	
	bool composite_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (next_context != nullptr)
			return next_context->handle_keyboard_event(key, acc);
		return false;
	}
}