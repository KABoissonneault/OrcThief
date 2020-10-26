#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/object/camera.h"
#include "egfx/window.h"

namespace ot::dedit::selection
{
	class edge_context : public context
	{
		map const* current_map;
		egfx::object::camera_cref main_camera;
		egfx::window const* main_window;

		entity_id selected_brush;
		egfx::face::id selected_face;
		egfx::half_edge::id selected_edge;

		std::optional<math::point3f> local_split;

	public:
		edge_context(map const& current_map
			, egfx::object::camera_cref main_camera
			, egfx::window const& main_window
			, entity_id selected_brush
			, egfx::face::id selected_face
			, egfx::half_edge::id selected_edge
		);

		virtual void update(action::accumulator& acc, input::frame_input& input) override;
	};
}
