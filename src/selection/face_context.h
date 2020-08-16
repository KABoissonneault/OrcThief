#pragma once

#include "selection/context.h"
#include "map.h"

#include "graphics/scene.h"
#include "graphics/window.h"

namespace ot::selection
{
	class face_context : public composite_context
	{
		map const* current_map;
		graphics::scene const* current_scene;
		graphics::window const* main_window;

		size_t selected_brush;
		graphics::face::id selected_face;
		graphics::half_edge::id hovered_edge = graphics::half_edge::id::none;

		void detect_hovered_edge();

	public:
		face_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush, graphics::face::id selected_face);

		virtual void update(math::seconds dt) override;
		virtual void render(graphics::node::manual& m) override;

		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& e) override;

		virtual void get_debug_string(std::string& s) const override;
	};
}
