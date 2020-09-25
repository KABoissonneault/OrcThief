#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/scene.h"
#include "egfx/window.h"

#include <optional>

namespace ot::dedit::selection
{
	// The context at the root of the application. Handles object selection and global events
	class base_context : public composite_context
	{
		map const* current_map;
		egfx::scene const* current_scene;
		egfx::window const* main_window;
		std::optional<size_t> selected_brush;

		void do_selection();

		void select_brush(size_t brush);
		void deselect_brush();

	public:
		base_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window) noexcept
			: current_map(&current_map)
			, current_scene(&current_scene)
			, main_window(&main_window)
		{

		}

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
		virtual void update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input) override;
		virtual void get_debug_string(std::string& s) const override;
	};
}
