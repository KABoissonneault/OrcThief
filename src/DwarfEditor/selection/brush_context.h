#pragma once

#include "selection/context.h"
#include "map.h"
#include "imgui/matrix.h"

#include "egfx/object/camera.h"
#include "egfx/window.h"

#include <optional>

namespace ot::dedit::selection
{
	class brush_context : public composite_context
	{
		map const* current_map;
		egfx::object::camera_cref main_camera;
		egfx::window const* main_window;

		entity_id selected_brush;
		egfx::face::id hovered_face = egfx::face::id::none;
		imgui::matrix object_matrix;
		bool is_editing = false;

		enum class operation_type
		{
			translate,
			rotation,
			scale,
			face_selection,
		};

		operation_type operation = operation_type::translate;
		bool use_world_manipulation = false;

		// Returns whether any manipulation input is active
		bool operation_window(action::accumulator& acc);
		bool draw_gizmo();
		brush const& get_brush() const;

	public:
		brush_context(map const& current_map, egfx::object::camera_cref main_camera, egfx::window const& main_window, entity_id selected_brush) noexcept;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;

		virtual void update(action::accumulator& acc, input::frame_input& input) override;

		virtual bool is_selected(entity_id id) const noexcept override { return selected_brush == id; }
	};
}
