#include "selection/base_context.h"

#include "selection/brush_context.h"
#include "selection/brush_common.h"

#include "egfx/object/camera.h"

namespace ot::dedit::selection
{
	void base_context::update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input)
	{		
		composite_context::update(m, acc, input);

		// Handle left-click selection
		if (input.consume_left_click())
		{
			math::ray const r = get_mouse_ray(*main_window, current_scene->get_camera());

			auto const result = current_scene->raycast_objects(r);

			for (egfx::node::object_id const hit_object : result)
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
		}
		else if (next_context != nullptr && input.consume_right_click())
		{
			next_context.reset();
		}
	}

	bool base_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		// Handle Escape first
		if (key.keysym.scancode == SDL_SCANCODE_ESCAPE && key.state == SDL_RELEASED)
		{
			next_context.reset();
			return true;
		}

		return composite_context::handle_keyboard_event(key, acc);
	}

	void base_context::get_debug_string(std::string& s) const 
	{ 
		if (next_context != nullptr)
		{
			composite_context::get_debug_string(s);
		}
		else
		{
			s += "Left-click on a Brush to select it\n";
		}
	}
}
