#pragma once

#include "selection/context.h"

#include "imgui/matrix.h"

#include "egfx/scene.fwd.h"
#include "egfx/window_type.h"

namespace ot::dedit::selection
{
	class face_split_context : public context
	{
		map const* current_map;
		egfx::window const* main_window;

		entity_id selected_brush;
		egfx::face::id selected_face;

		imgui::matrix plane_transform;

		int current_operation;

		void operation_window(action::accumulator& acc);

		void do_split(action::accumulator& acc);

		brush const& get_brush() const;

	public:
		face_split_context(map const& current_map, 
			egfx::window const& main_window
			, entity_id selected_brush
			, egfx::face::id selected_face
		);

		virtual void update(action::accumulator& acc, input::frame_input& input) override;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;
	};
}
