#include "selection/brush_context.h"

#include "datablock.h"
#include "input.h"

#include "egfx/camera.h"
#include "egfx/window.h"

#include "selection/face_context.h"

namespace ot::selection
{
	namespace
	{
		egfx::face::id get_closest_face(math::point3d camera_wpos, math::ray const& mouse_ray, math::transformation const& brush_transform, egfx::mesh_definition const& mesh)
		{
			egfx::face::id current_face = egfx::face::id::none;
			double current_distance_sq = DBL_MAX;
			for (egfx::face::cref const face : mesh.get_faces())
			{
				math::plane const local_plane = face.get_plane();
				math::plane const world_plane = transform(local_plane, brush_transform);

				auto const result = mouse_ray.intersects(world_plane);
				if (result)
				{
					math::point3d const intersection = *result;
					math::point3d const local_intersection = detransform(intersection, invert(brush_transform));
					if (face.is_on_face(local_intersection))
					{
						double const intersection_distance_sq = (camera_wpos - intersection).norm_squared();
						if (float_cmp(intersection_distance_sq, current_distance_sq) < 0)
						{
							current_distance_sq = intersection_distance_sq;
							current_face = face.get_id(); // nearest face that the cursor is hovering
						}
					}
				}
			}

			return current_face;
		}
	}

	brush_context::brush_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window, size_t selected_brush) noexcept
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
	{
		select(selected_brush);
	}

	void brush_context::update()
	{
		hovered_face = egfx::face::id::none;

		if (next_context == nullptr && has_focus(*main_window))
		{
			detect_hovered_face();
		}

		composite_context::update();
	}

	void brush_context::detect_hovered_face()
	{
		int mouse_x, mouse_y;
		input::get_mouse_position(mouse_x, mouse_y);

		double const viewport_x = static_cast<double>(mouse_x) / get_width(*main_window);
		double const viewport_y = static_cast<double>(mouse_y) / get_height(*main_window);
		egfx::camera const& camera = current_scene->get_camera();
		math::ray const mouse_ray = get_world_ray_from_viewport(camera, viewport_x, viewport_y);

		brush const& brush = current_map->get_brushes()[selected_brush];
		auto const& mesh = *brush.mesh_def;

		math::transformation const t = brush.get_world_transform(math::transformation::identity());
		hovered_face = get_closest_face(get_position(camera), mouse_ray, t, mesh);
	}

	void brush_context::render(egfx::node::manual& m)
	{
		brush const& b = current_map->get_brushes()[selected_brush];
		math::transformation const t = b.get_world_transform(math::transformation::identity());

		m.add_wiremesh(datablock::overlay_unlit, *b.mesh_def, t);

		if (hovered_face != egfx::face::id::none && next_context == nullptr)
		{
			m.add_face(datablock::overlay_unlit_transparent_light, { *b.mesh_def, hovered_face }, t);
		}

		composite_context::render(m);

		auto const vertices = b.mesh_def->get_vertices();
		for (egfx::vertex::cref const vertex : vertices)
		{
			math::point3d const vertex_pos = transform(vertex.get_position(), t);
			math::transformation const vt{ vector_from_origin(vertex_pos), math::quaternion::identity(), 0.04 };
			m.add_mesh(datablock::overlay_unlit_vertex, egfx::mesh_definition::get_cube(), vt);
		}
	}

	bool brush_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (composite_context::handle_keyboard_event(key, acc))
			return true;

		SDL_Keysym const& keysym = key.keysym;
		if (keysym.scancode == SDL_SCANCODE_TAB && key.state == SDL_RELEASED)
		{
			if (keysym.mod & KMOD_LSHIFT)
			{
				select_previous();
			}
			else
			{
				select_next();
			}
			
			return true;
		}

		return false;
	}

	bool brush_context::handle_mouse_button_event(SDL_MouseButtonEvent const& mouse, action::accumulator& acc)
	{
		if (composite_context::handle_mouse_button_event(mouse, acc))
			return true;

		if (mouse.button == 1 && mouse.state == SDL_RELEASED && hovered_face != egfx::face::id::none)
		{
			next_context.reset(new face_context(*current_map, *current_scene, *main_window, selected_brush, hovered_face));
			return true;
		}

		if (mouse.button == 3 && mouse.state == SDL_RELEASED && next_context != nullptr)
		{
			next_context.reset();
			return true;
		}

		return false;
	}

	void brush_context::select(size_t brush_idx)
	{
		selected_brush = brush_idx;
	}

	void brush_context::select_next()
	{
		if (selected_brush == current_map->get_brushes().size() - 1)
		{
			select(0);
		}
		else
		{
			select(selected_brush + 1);
		}
	}

	void brush_context::select_previous()
	{
		if (selected_brush == 0)
		{
			select(current_map->get_brushes().size() - 1);
		}
		else
		{
			select(selected_brush - 1);
		}
	}

	void brush_context::get_debug_string(std::string& s) const
	{		
		if (next_context != nullptr)
		{
			composite_context::get_debug_string(s);
		}
		else 
		{			
			s += "Left-click on a face to select it\n";
			s += "Right-click to deselect the brush\n";
			s += "Selected brush: " + std::to_string(selected_brush) + "\n";
			if (hovered_face != egfx::face::id::none)
			{
				s += "Hovered face: " + std::to_string(static_cast<size_t>(hovered_face)) + "\n";
			}
		}
	}
}
