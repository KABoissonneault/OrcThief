#pragma once

#include "selection/context.h"
#include "map.h"

#include "graphics/scene.h"
#include "graphics/window.h"

namespace ot::selection
{
	class brush_context : public context
	{
		map* current_map;
		graphics::scene const* current_scene;
		graphics::window const* main_window;

		size_t selected_brush;
		ot::graphics::node::static_mesh selection_node;

		void select(size_t brush_idx);
		void select_next();
		void select_previous();

	public:
		brush_context(map& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush) noexcept;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse) override;
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse) override;

		virtual void get_debug_string(std::string& s) const override;
	};
}
