#include "selection/base_context.h"

#include "selection/brush_context.h"

#include "graphics/camera.h"

namespace ot::selection
{
	void base_context::update(math::seconds dt)
	{
		if (next_context != nullptr)
		{
			next_context->update(dt);
		}
	}

	bool base_context::handle_keyboard_event(SDL_KeyboardEvent const& key)
	{
		// Handle Escape first
		if (key.keysym.scancode == SDL_SCANCODE_ESCAPE && key.state == SDL_RELEASED)
		{
			next_context.reset();
			return true;
		}

		return next_context != nullptr && next_context->handle_keyboard_event(key);
	}

	bool base_context::handle_mouse_button_event(SDL_MouseButtonEvent const& mouse)
	{
		if (next_context != nullptr && next_context->handle_mouse_button_event(mouse))
		{
			return true;
		}

		// Left-click for selection
		if (mouse.button == 1 && mouse.state == SDL_RELEASED)
		{
			int const width = get_width(*main_window);
			int const height = get_height(*main_window);

			double const viewport_x = static_cast<double>(mouse.x) / width;
			double const viewport_y = static_cast<double>(mouse.y) / height;

			math::ray const r = get_world_ray_from_viewport(current_scene->get_camera(), viewport_x, viewport_y);
			auto const result = current_scene->raycast_objects(r);

			for (graphics::node::object_id const hit_object : result)
			{
				std::span<brush const> const brushes = current_map->get_brushes();
				auto const found_brush = std::find_if(brushes.begin(), brushes.end(), [hit_object](brush const& b)
				{
					return b.node.contains(hit_object);
				});
				if (found_brush == brushes.end())
					continue;

				size_t const hit_brush_idx = std::distance(brushes.begin(), found_brush);
				next_context.reset(new brush_context(*current_map, *current_scene, *main_window, hit_brush_idx));
			}

			return true;
		}

		return false;
	}

	bool base_context::handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse)
	{
		return next_context != nullptr && next_context->handle_mouse_motion_event(mouse);
	}

	void base_context::get_debug_string(std::string& s) const 
	{ 
		if (next_context != nullptr)
		{
			next_context->get_debug_string(s);
		}
		else
		{
			s += "Left-click on a Brush to select it\n";
		}
	}
}
