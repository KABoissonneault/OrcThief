#include "selection/brush_context.h"

namespace ot::selection
{
	brush_context::brush_context(map& current_map, graphics::scene const& current_scene, graphics::window const& main_window, size_t selected_brush) noexcept
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
	{
		select(selected_brush);
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
		(void)mouse;
		return false;
	}

	bool brush_context::handle_mouse_motion_event(SDL_MouseMotionEvent const& mouse)
	{
		(void)mouse;
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
}