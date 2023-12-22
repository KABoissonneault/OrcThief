#include "selection/base_context.h"

#include "selection/brush_context.h"
#include "selection/brush_common.h"

#include "egfx/object/camera.h"

#include <imgui.h>
#include <format>

namespace ot::dedit::selection
{
	namespace
	{
		bool hits_brush(math::ray const& mouse_ray, math::transform_matrix const& brush_transform, egfx::mesh_definition const& mesh)
		{
			for (egfx::face::cref const face : mesh.get_faces())
			{
				math::plane const local_plane = face.get_plane();
				math::plane const world_plane = transform(local_plane, brush_transform);

				auto const result = mouse_ray.intersects(world_plane);
				if (result)
				{
					math::point3f const intersection = *result;
					math::point3f const local_intersection = transform(intersection, invert(brush_transform));
					if (face.is_on_face(local_intersection))
					{
						return true;
					}
				}
			}

			return false;
		}
	}

	void base_context::do_selection()
	{
		math::ray const r = get_mouse_ray(*main_window, current_scene->get_camera());

		auto const result = current_scene->raycast_objects(r);

		for (egfx::node::object_id const hit_object : result)
		{
			std::span<brush const> const brushes = current_map->get_brushes();
			auto const found_brush = std::find_if(brushes.begin(), brushes.end(), [hit_object](brush const& b)
			{
				return b.get_node().contains(hit_object);
			});

			if (found_brush == brushes.end())
				continue;		

			entity_id const hit_brush_id = found_brush->get_id();
			if (hit_brush_id == selected_brush)
				continue;
			
			brush const& b = *found_brush;

			if (hits_brush(r, b.get_world_transform(current_map->get_brush_root_world_transform()), b.get_mesh_def()))
			{
				select_brush(hit_brush_id);
				break;
			}
		}
	}

	void base_context::select_brush(entity_id brush)
	{
		next_context.reset(new brush_context(*current_map, current_scene->get_camera(), *main_window, brush));
		selected_brush = brush;
	}

	void base_context::deselect_brush()
	{
		next_context.reset();
		selected_brush.reset();
	}
	
	void base_context::start_frame()
	{
		if (selected_brush)
		{
			// If the brush was externally deleted, go back to base context
			if (current_map->find_brush(*selected_brush) == nullptr)
				deselect_brush();
		}

		composite_context::start_frame();
	}

	void base_context::update(action::accumulator& acc, input::frame_input& input)
	{		
		if (selected_brush)
		{
			// If the brush was deleted during event handling, go back to base context
			if (current_map->find_brush(*selected_brush) == nullptr)
				deselect_brush();
		}

		composite_context::update(acc, input);

		// Handle left-click selection
		if (input.consume_left_click())
		{
			do_selection();
		}
		else if (next_context != nullptr && input.consume_right_click())
		{
			deselect_brush();
		}

		if (!current_map->get_brushes().empty())
		{
			ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
			ImVec2 const initial_pos(main_viewport->Pos.x + main_viewport->Size.x - 200, main_viewport->Pos.y + main_viewport->Size.y * 0.2f);
			ImVec2 const size_min(192, ImGui::GetTextLineHeightWithSpacing() * 4);
			ImVec2 const size_max(192, ImGui::GetTextLineHeightWithSpacing() * 16);
			ImGui::SetNextWindowPos(initial_pos, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSizeConstraints(size_min, size_max);
			if (ImGui::Begin("Brushes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				for (brush const& b : current_map->get_brushes())
				{
					ImGui::Selectable(std::format("{}##{}", b.get_name(), as_int(b.get_id())).c_str());

					if (ImGui::IsItemActivated())
					{
						if (selected_brush != b.get_id())
							select_brush(b.get_id());
					}
				}
			}

			ImGui::End();
		}
	}

	bool base_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		// Handle Escape first
		if (key.keysym.scancode == SDL_SCANCODE_ESCAPE && key.state == SDL_PRESSED)
		{
			deselect_brush();
			return true;
		}

		return composite_context::handle_keyboard_event(key, acc);
	}
}
