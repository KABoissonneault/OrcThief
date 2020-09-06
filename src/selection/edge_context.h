#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/scene.h"
#include "egfx/window.h"

namespace ot::selection
{
	class edge_context : public context
	{
		map const* current_map;
		egfx::scene const* current_scene;
		egfx::window const* main_window;

		size_t selected_brush;
		egfx::face::id selected_face;
		egfx::half_edge::id selected_edge;

		std::optional<math::point3d> local_split;

		void preview_edge_split();

	public:
		edge_context(map const& current_map
			, egfx::scene const& current_scene
			, egfx::window const& main_window
			, size_t selected_brush
			, egfx::face::id selected_face
			, egfx::half_edge::id selected_edge
		);

		virtual void update() override;
		virtual void render(egfx::node::manual& m) override;
		virtual bool handle_mouse_button_event(SDL_MouseButtonEvent const& e, action::accumulator& acc) override;
		virtual void get_debug_string(std::string& s) const override;
	};
}
