#pragma once

#include "selection/context.h"
#include "map.h"

#include "graphics/scene.h"
#include "graphics/window.h"

namespace ot::selection
{
	class edge_context : public context
	{
		map const* current_map;
		graphics::scene const* current_scene;
		graphics::window const* main_window;

		size_t selected_brush;
		graphics::face::id selected_face;
		graphics::half_edge::id selected_edge;

		std::optional<math::point3d> local_split;

		void preview_edge_split();

	public:
		edge_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush, graphics::face::id selected_face, graphics::half_edge::id selected_edge);

		virtual void update(math::seconds dt) override;
		virtual void render(graphics::node::manual& m) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& e, action::accumulator& acc) override;
	};
}
