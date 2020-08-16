#include "selection/face_context.h"

#include "selection/edge_context.h"

#include "datablock.h"
#include "input.h"

#include "graphics/camera.h"

namespace ot::selection
{
	face_context::face_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush, graphics::face::id selected_face)
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
	{

	}

	void face_context::update(math::seconds dt)
	{
		(void)dt;

		if (next_context == nullptr && has_focus(*main_window))
		{
			detect_hovered_edge();
		}

		composite_context::update(dt);
	}

	void face_context::detect_hovered_edge()
	{
		int mouse_x, mouse_y;
		input::get_mouse_position(mouse_x, mouse_y);

		double const viewport_x = static_cast<double>(mouse_x) / get_width(*main_window);
		double const viewport_y = static_cast<double>(mouse_y) / get_height(*main_window);
		graphics::camera const& camera = current_scene->get_camera();
		math::ray const mouse_ray = get_world_ray_from_viewport(camera, viewport_x, viewport_y);

		brush const& brush = current_map->get_brushes()[selected_brush];
		auto const& mesh = *brush.mesh_def;

		math::transformation const t = brush.get_world_transform(math::transformation::identity());

		auto const face = mesh.get_face(selected_face);
		math::plane const local_plane = face.get_plane();
		math::plane const world_plane = transform(local_plane, t);

		auto const intersection_result = mouse_ray.intersects(world_plane);
		if (!intersection_result)
			return;

		math::point3d const& intersection_point = *intersection_result;
		math::point3d const local_point = detransform(intersection_point, invert(t));

		// Pick the closest edge
		double current_distance_sq = DBL_MAX;
		for (graphics::half_edge::cref const he : face.get_half_edges())
		{
			math::line const edge = he.get_line();
			math::point3d const center = midpoint(edge.a, edge.b);
			double const distance_sq = (local_point - center).norm_squared();
			if (float_cmp(distance_sq, current_distance_sq) < 0)
			{
				current_distance_sq = distance_sq;
				hovered_edge = he.get_id();
			}
		}
	}

	void face_context::render(graphics::node::manual& m)
	{
		brush const& b = current_map->get_brushes()[selected_brush];
		math::transformation const t = b.get_world_transform(math::transformation::identity());

		m.add_face(datablock::overlay_unlit_transparent_heavy, b.mesh_def->get_face(selected_face), t);

		if (hovered_edge != graphics::half_edge::id::none)
		{
			math::line const local_line = b.mesh_def->get_half_edge(hovered_edge).get_line();
			m.add_line(datablock::overlay_unlit_edge, transform(local_line, t));
		}

		composite_context::render(m);
	}

	bool face_context::handle_mouse_button_event(SDL_MouseButtonEvent const& e)
	{
		if (composite_context::handle_mouse_button_event(e))
			return true;

		if (e.button == 1 && e.state == SDL_RELEASED && hovered_edge != graphics::half_edge::id::none)
		{
			next_context.reset(new edge_context(*current_map, *current_scene, *main_window, selected_brush, selected_face, hovered_edge));
			return true;
		}

		if (e.button == 3 && e.state == SDL_RELEASED && next_context != nullptr)
		{
			next_context.reset();
			return true;
		}

		return false;
	}

	void face_context::get_debug_string(std::string& s) const
	{
		s += "Selected face: " + std::to_string(static_cast<size_t>(selected_face)) + "\n";
		composite_context::get_debug_string(s);
	}
}
