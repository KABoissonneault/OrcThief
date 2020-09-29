#include "selection/face_context.h"

#include "selection/edge_context.h"
#include "selection/brush_common.h"

#include "datablock.h"
#include "input.h"

#include "egfx/object/camera.h"

namespace ot::dedit::selection
{
	namespace
	{
		egfx::half_edge::id detect_hovered_edge(math::ray const& mouse_ray, egfx::face::cref face, math::transform_matrix const& t)
		{
			math::plane const local_plane = face.get_plane();
			math::plane const world_plane = transform(local_plane, t);

			auto const intersection_result = mouse_ray.intersects(world_plane);
			if (!intersection_result)
			{				
				return egfx::half_edge::id::none;
			}

			math::point3f const& intersection_point = *intersection_result;
			math::point3f const local_point = transform(intersection_point, invert(t));

			// Pick the closest edge
			float current_distance_sq = FLT_MAX;
			egfx::half_edge::id current_edge = egfx::half_edge::id::none;
			for (egfx::half_edge::cref const he : face.get_half_edges())
			{
				math::line const edge = he.get_line();
				math::point3f const center = midpoint(edge.a, edge.b);
				float const distance_sq = (local_point - center).norm_squared();
				if (float_cmp(distance_sq, current_distance_sq) < 0)
				{
					current_distance_sq = distance_sq;
					current_edge = he.get_id();
				}
			}

			return current_edge;
		}
	}

	face_context::face_context(map const& current_map
		, egfx::scene const& current_scene
		, egfx::window const& main_window
		, entity_id selected_brush
		, egfx::face::id selected_face)
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
	{

	}

	void face_context::update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input)
	{
		hovered_edge = egfx::half_edge::id::none;

		brush const& b = *current_map->find_brush(selected_brush);
		math::transform_matrix const t = b.get_world_transform(math::transform_matrix::identity());

		// Find the hovered edge
		if (next_context == nullptr)
		{
			if (has_focus(*main_window))
			{
				math::ray const mouse_ray = get_mouse_ray(*main_window, current_scene->get_camera());
				hovered_edge = detect_hovered_edge(mouse_ray, b.mesh_def->get_face(selected_face), t);
			}
		}	
		
		// Add overlays
		m.add_face(datablock::overlay_unlit_transparent_heavy, b.mesh_def->get_face(selected_face), t);

		if (hovered_edge != egfx::half_edge::id::none)
		{
			math::line const local_line = b.mesh_def->get_half_edge(hovered_edge).get_line();
			m.add_line(datablock::overlay_unlit_edge, transform(local_line, t));
		}

		composite_context::update(m, acc, input);

		// Handle input
		if(next_context == nullptr)
		{
			if (hovered_edge != egfx::half_edge::id::none && input.consume_left_click())
			{
				next_context.reset(new edge_context(*current_map, *current_scene, *main_window, selected_brush, selected_face, hovered_edge));
			}
		}
		else if (input.consume_right_click())
		{
			next_context.reset();
		}
	}
}
