#pragma once

#include "selection/context.h"
#include "map.h"

#include "graphics/scene.h"
#include "graphics/window.h"

namespace ot::selection
{
	class brush_context : public context
	{
		map const* current_map;
		graphics::scene const* current_scene;
		graphics::window const* main_window;

		size_t selected_brush;
		graphics::face::id hovered_face = graphics::face::id::none;

		int mouse_x;
		int mouse_y;

		void select(size_t brush_idx);
		void select_next();
		void select_previous();

	public:
		brush_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush) noexcept;

		virtual void update(math::seconds dt) override;
		virtual void render(graphics::node::manual& m) override;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& mouse) override;
		virtual bool handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse) override;

		virtual void get_debug_string(std::string& s) const override;
	};
}
