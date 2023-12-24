#include "selection/edge_context.h"

#include "selection/brush_common.h"

#include "datablock.h"
#include "input.h"
#include "action/brush.h"

#include "egfx/object/camera.h"

#include "math/vector3.h"
#include "math/quaternion.h"

#include <im3d.h>

namespace ot::dedit::selection
{
	edge_context::edge_context(map const& current_map
		, egfx::object::camera_cref main_camera
		, egfx::window const& main_window
		, entity_id selected_brush
		, egfx::face::id selected_face
		, egfx::half_edge::id selected_edge)
		: current_map(&current_map)
		, main_camera(main_camera)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
		, selected_edge(selected_edge)
	{

	}

	void edge_context::update(action::accumulator& acc, input::frame_input& input)
	{
		local_split = std::nullopt;

		if (!has_focus(*main_window))
			return;

		brush const& b = *current_map->find_brush(selected_brush);
		egfx::mesh_definition const& mesh_def = b.get_mesh_def();
		math::transform_matrix const t = b.get_world_transform();
		
		egfx::half_edge::cref const edge = mesh_def.get_half_edge(selected_edge);
		math::line const local_line = edge.get_line();

		// Find the local split
		{
			egfx::face::cref const face = mesh_def.get_face(selected_face);
			math::plane const face_plane = transform(face.get_plane(), t);
			if (auto const intersection_result = get_mouse_ray(*main_window, main_camera).intersects(face_plane))
			{
				math::point3f const local_point = transform(*intersection_result, invert(t));				
				local_split = clamped_project(local_line, local_point);
			}
		}		

		// Add overlays
		math::line const world_line = transform(local_line, t);
		Im3d::DrawLine(world_line.a, world_line.b, 5.f, Im3d::Color_Red);

		if (local_split)
		{
			math::point3f const vertex_pos = transform(*local_split, t);
			Im3d::DrawPoint(vertex_pos, 10.f, Im3d::Color_Blue);
		}

		// Handle click
		if (local_split && input.consume_left_click())
		{
			math::point3f const p = *local_split;
			if (!float_eq(p, local_line.a) && !float_eq(p, local_line.b)) // If the point is at the extremities, don't split
			{
				acc.emplace_action<action::split_brush_edge>(b, selected_edge, p);
			}	
		}
	}
}
