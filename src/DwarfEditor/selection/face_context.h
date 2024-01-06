#pragma once

#include "selection/context.h"
#include "map.h"

#include "egfx/object/camera.h"
#include "egfx/window.h"

namespace ot::dedit::selection
{
	class face_context : public composite_context
	{
		map const* current_map;
		egfx::camera_cref main_camera;
		egfx::window const* main_window;

		entity_id selected_brush;
		egfx::face::id selected_face;
		egfx::half_edge::id hovered_edge = egfx::half_edge::id::none;

		enum class operation_type
		{
			edge_selection,
			split,
		};

		operation_type current_operation = operation_type::edge_selection;

		void operation_window();

		brush const& get_brush() const;

	public:
		face_context(map const& current_map, egfx::camera_cref main_camera, egfx::window const& main_window, entity_id selected_brush, egfx::face::id selected_face);

		virtual void update(action::accumulator& acc, input::frame_input& input) override;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
	};
}
