#include "selection/brush_context.h"

#include "graphics/camera.h"
#include "graphics/window.h"

namespace ot::selection
{
	namespace
	{
		std::optional<graphics::face::id> get_closest_face(math::point3d camera_wpos, math::ray const& mouse_ray, math::transformation const& brush_transform, graphics::mesh_definition const& mesh)
		{
			graphics::face::id current_face = graphics::face::id::none;
			double current_distance_sq = DBL_MAX;
			for (graphics::face::id const face : mesh.get_faces())
			{
				math::plane const local_plane = get_plane(mesh, face);
				math::plane const world_plane = transform(local_plane, brush_transform);

				auto const result = mouse_ray.intersects(world_plane);
				if (result)
				{
					math::point3d const intersection = *result;
					math::point3d const local_intersection = transform(intersection, invert(brush_transform));
					if (is_on_face(mesh, face, local_intersection))
					{
						double const intersection_distance_sq = (camera_wpos - intersection).norm_squared();
						if (float_cmp(intersection_distance_sq, current_distance_sq) < 0)
						{
							current_face = face; // nearest face that the cursor is hovering
						}
					}
				}
			}

			if (current_face == graphics::face::id::none)
			{
				return {};
			} 
			else
			{
				return current_face;
			}
		}
	}

	brush_context::brush_context(map& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush) noexcept
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
	{
		select(selected_brush);
	}

	void brush_context::update(math::seconds dt)
	{
		(void)dt;

		
	}

	bool brush_context::handle_keyboard_event(SDL_KeyboardEvent const& key)
	{
		SDL_Keysym const& keysym = key.keysym;
		if (keysym.scancode == SDL_SCANCODE_TAB && key.state == SDL_RELEASED)
		{
			if (keysym.mod & KMOD_LSHIFT)
			{
				select_previous();
			} else
			{
				select_next();
			}
			
			return true;
		}

		return false;
	}

	bool brush_context::handle_mouse_button_event(SDL_MouseButtonEvent const& mouse)
	{
		if (mouse.state == SDL_RELEASED && mouse.button == 3)
		{
			double const viewport_x = static_cast<double>(mouse.x) / get_width(*main_window);
			double const viewport_y = static_cast<double>(mouse.y) / get_height(*main_window);
			graphics::camera const& camera = current_scene->get_camera();
			math::ray const mouse_ray = get_world_ray_from_viewport(camera, viewport_x, viewport_y);

			brush& brush = current_map->get_brushes()[selected_brush];
			auto const& mesh = brush.mesh_def;

			math::quaternion const world_rot = brush.node.get_rotation();
			math::point3d const local_pos = brush.node.get_position();
			math::vector3d const world_displacement = vector_from_origin(local_pos);

			auto const result = get_closest_face(get_position(camera), mouse_ray, { world_displacement, world_rot }, mesh);
			if (result)
			{
				// TODO: face mesh
				hovered_face = *result;
			} else
			{
				hovered_face = graphics::face::id::none;
			}

			return true;
		}

		return false;
	}

	bool brush_context::handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse)
	{
		// Just taking the info, not actually handling the event
		mouse_x = mouse.x;
		mouse_y = mouse.y;

		return false;
	}

	void brush_context::select(size_t brush_idx)
	{
		selection_node = {}; // need to destroy the "SelectedBrush" object first

		brush& brush = current_map->get_brushes()[brush_idx];
		selection_node = graphics::node::create_static_wireframe_mesh(brush.node, "SelectedBrush", brush.mesh_def);
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
		s += "Selected brush: " + std::to_string(selected_brush) + "\n";
		if (hovered_face != graphics::face::id::none)
		{
			s += "Hovered face: " + std::to_string(static_cast<size_t>(hovered_face)) + "\n";
 		}
	}
}