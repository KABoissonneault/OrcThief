#pragma once

#include "selection/context.h"
#include "map.h"
#include "imgui/matrix.h"

#include "egfx/object/camera.h"
#include "egfx/window.h"
#include "egfx/material.h"

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

		std::vector<egfx::material_handle_t> cached_materials;
		imgui::matrix object_local_matrix;
		bool was_editing = false;
		bool was_text_editing = false;
		bool was_gizmo_editing = false;

		enum class operation_type
		{
			translate,
			rotation,
			scale,
			face_selection,
		};

		enum class vertex_debug_type
		{
			none,
			uv,
		};

		operation_type operation = operation_type::translate;
		vertex_debug_type vertex_debug = vertex_debug_type::none;
		bool use_world_manipulation = false;
		
		// Returns whether any manipulation input is active
		bool operation_window(action::accumulator& acc, math::transform_matrix const& original_local_transform);
		bool draw_gizmo(imgui::matrix& object_world_matrix);

		void properties_window(action::accumulator& acc);

		brush const& get_brush() const;

		void draw_immediate_scene(egfx::mesh_definition const& mesh_def, math::transform_matrix const& t);

	public:
		brush_context(map const& current_map, egfx::object::camera_cref main_camera, egfx::window const& main_window, entity_id selected_brush) noexcept;

		virtual bool handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc) override;

		virtual void update(action::accumulator& acc, input::frame_input& input) override;

		virtual bool is_selected(entity_id id) const noexcept override { return selected_brush == id; }
	};
}
