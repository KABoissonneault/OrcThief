#include "selection/edge_context.h"

#include "datablock.h"
#include "input.h"

#include "graphics/camera.h"

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::selection
{
	edge_context::edge_context(map const& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush, graphics::face::id selected_face, graphics::half_edge::id selected_edge)
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
		, selected_edge(selected_edge)
	{

	}

	void edge_context::update(math::seconds dt)
	{
		(void)dt;

		preview_edge_split();
	}

	void edge_context::preview_edge_split()
	{
		if (!has_focus(*main_window))
			return;

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

		auto const half_edge = mesh.get_half_edge(selected_edge);
		math::line const line = half_edge.get_line();
		local_split = clamped_project(line, local_point);
	}

	void edge_context::render(graphics::node::manual& m)
	{
		brush const& b = current_map->get_brushes()[selected_brush];
		math::transformation const t = b.get_world_transform(math::transformation::identity());

		math::line const local_line = b.mesh_def->get_half_edge(selected_edge).get_line();
		m.add_line(datablock::overlay_unlit_edge, transform(local_line, t));

		if (local_split)
		{
			math::point3d const vertex_pos = transform(*local_split, t);
			math::transformation const cube_transform{ vector_from_origin(vertex_pos), math::quaternion::identity(), 0.04 };
			m.add_mesh(datablock::overlay_unlit_vertex_transparent, graphics::mesh_definition::get_cube(), cube_transform);
		}
	}
}