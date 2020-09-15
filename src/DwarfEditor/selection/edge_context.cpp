#include "selection/edge_context.h"

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

	void edge_context::update()
	{
		local_split = std::nullopt;
		preview_edge_split();
	}

	void edge_context::preview_edge_split()
	{
		if (!has_focus(*main_window))
			return;

		int mouse_x, mouse_y;
		input::mouse::get_position(mouse_x, mouse_y);

		float const viewport_x = static_cast<float>(mouse_x) / get_width(*main_window);
		float const viewport_y = static_cast<float>(mouse_y) / get_height(*main_window);
		math::ray const mouse_ray = current_scene->get_camera().get_world_ray(viewport_x, viewport_y);

		brush const& brush = current_map->get_brushes()[selected_brush];
		auto const& mesh = *brush.mesh_def;

		math::transformation const t = brush.get_world_transform(math::transformation::identity());

		auto const face = mesh.get_face(selected_face);
		math::plane const local_plane = face.get_plane();
		math::plane const world_plane = transform(local_plane, t);

		auto const intersection_result = mouse_ray.intersects(world_plane);
		if (!intersection_result)
			return;

		math::point3f const& intersection_point = *intersection_result;
		math::point3f const local_point = detransform(intersection_point, invert(t));

		auto const half_edge = mesh.get_half_edge(selected_edge);
		math::line const line = half_edge.get_line();
		local_split = clamped_project(line, local_point);
	}

	void edge_context::render(egfx::node::manual& m)
	{
		brush const& b = current_map->get_brushes()[selected_brush];
		math::transformation const t = b.get_world_transform(math::transformation::identity());

		math::line const local_line = b.mesh_def->get_half_edge(selected_edge).get_line();
		m.add_line(datablock::overlay_unlit_edge, transform(local_line, t));

		if (local_split)
		{
			math::point3f const vertex_pos = transform(*local_split, t);
			math::transformation const cube_transform{ vector_from_origin(vertex_pos), math::quaternion::identity(), 0.04f };
			m.add_mesh(datablock::overlay_unlit_vertex_transparent, egfx::mesh_definition::get_cube(), cube_transform);
		}
	}

	bool edge_context::handle_mouse_button_event(SDL_MouseButtonEvent const& e, action::accumulator& acc)
	{
		if (e.button == 1 && e.state == SDL_RELEASED)
		{
			if (!local_split)
				return false;

			brush const& b = current_map->get_brushes()[selected_brush];
			egfx::half_edge::cref const edge = b.mesh_def->get_half_edge(selected_edge);
			math::line const local_line = edge.get_line();

			math::point3f const p = *local_split;
			if (float_eq(p, local_line.a) || float_eq(p, local_line.b)) // If the point is at the extremities, don't split
				return false;
						
			acc.emplace_brush_action<action::split_edge>(b, selected_edge, p);

			return true;
		}

		return false;
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
