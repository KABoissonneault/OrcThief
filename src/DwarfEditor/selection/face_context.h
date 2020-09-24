#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/scene.h"
#include "egfx/window.h"

namespace ot::dedit::selection
{
	class face_context : public composite_context
	{
		map const* current_map;
		egfx::scene const* current_scene;
		egfx::window const* main_window;

		size_t selected_brush;
		egfx::face::id selected_face;
		egfx::half_edge::id hovered_edge = egfx::half_edge::id::none;

	public:
		face_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window, size_t selected_brush, egfx::face::id selected_face);

		virtual void update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input) override;

		virtual void get_debug_string(std::string& s) const override;
	};
}
