#pragma once

#include "core/uptr.h"
#include "math/unit/time.h"

#include "egfx/node/manual.h"

#include "map.h"
#include "input.h"

#include "action/accumulator.h"

#include <SDL_events.h>

#include <string>

namespace ot::dedit::selection
{
	class context
	{
	public:
		virtual ~context() = 0;

		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) { (void)key, (void) acc; return false; }
		
		virtual void update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input) { (void)m, (void)acc, (void) input; }

		virtual bool is_selected(entity_id id) const noexcept { (void)id; return false; }

		virtual void get_debug_string(std::string& debug) const { (void)debug; }
	};

	class composite_context : public context
	{
	protected:
		uptr<context> next_context;

	public:
		// Returns true if the input was handled
		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
		
		virtual void update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input) override;

		virtual bool is_selected(entity_id id) const noexcept { return next_context != nullptr ? next_context->is_selected(id) : false; }

		virtual void get_debug_string(std::string& debug) const override;
	};
}