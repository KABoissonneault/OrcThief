#include "selection/edge_context.h"

#include "selection/brush_common.h"

#include "datablock.h"
#include "input.h"
#include "action/brush.h"

#include "egfx/object/camera.h"

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::dedit::selection
{
	edge_context::edge_context(map const& current_map
		, egfx::scene const& current_scene
		, egfx::window const& main_window
		, size_t selected_brush
		, egfx::face::id selected_face
		, egfx::half_edge::id selected_edge)
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
		, selected_edge(selected_edge)
	{

	}

	void edge_context::update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input)
	{
		local_split = std::nullopt;

		if (!has_focus(*main_window))
			return;

		brush const& b = current_map->get_brushes()[selected_brush];
		egfx::mesh_definition const& mesh_def = *b.mesh_def;
		math::transform_matrix const t = b.get_world_transform(math::transform_matrix::identity());
		
		egfx::half_edge::cref const edge = mesh_def.get_half_edge(selected_edge);
		math::line const local_line = edge.get_line();

		// Find the local split
		{
			egfx::object::camera_cref camera = current_scene->get_camera();
			egfx::face::cref const face = mesh_def.get_face(selected_face);
			math::plane const face_plane = transform(face.get_plane(), t);
			if (auto const intersection_result = get_mouse_ray(*main_window, camera).intersects(face_plane))
			{
				math::point3f const local_point = transform(*intersection_result, invert(t));				
				local_split = clamped_project(local_line, local_point);
			}
		}		

		// Add overlays
		m.add_line(datablock::overlay_unlit_edge, transform(local_line, t));

		if (local_split)
		{
			math::point3f const vertex_pos = transform(*local_split, t);
			auto const cube_transform = math::transform_matrix::from_components(vector_from_origin(vertex_pos), math::quaternion::identity(), 0.04f);
			m.add_mesh(datablock::overlay_unlit_vertex_transparent, egfx::mesh_definition::get_cube(), cube_transform);
		}

		// Handle click
		if (local_split && input.consume_left_click())
		{
			math::point3f const p = *local_split;
			if (!float_eq(p, local_line.a) && !float_eq(p, local_line.b)) // If the point is at the extremities, don't split
			{
				acc.emplace_brush_action<action::split_edge>(b, selected_edge, p);
			}	
		}
	}

	void edge_context::get_debug_string(std::string& s) const
	{
		s += "Left-click to split the edge\n";
		s += "CTRL+Z to undo\n";
		s += "Right-click to deselect the edge\n";
		s += "Selected brush: " + std::to_string(selected_brush) + "\n";
		s += "Selected face: " + std::to_string(static_cast<size_t>(selected_face)) + "\n";
		s += "Selected edge: " + std::to_string(static_cast<size_t>(selected_edge)) + "\n";
	}
}
