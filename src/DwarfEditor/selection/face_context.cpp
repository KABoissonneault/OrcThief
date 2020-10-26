#include "selection/face_context.h"

#include "selection/edge_context.h"
#include "selection/face_split_context.h"
#include "selection/brush_common.h"

#include "datablock.h"
#include "input.h"

#include "egfx/object/camera.h"
#include "egfx/immediate.h"

#include <imgui.h>
#include <im3d.h>

#include <fmt/format.h>

namespace ot::dedit::selection
{
	namespace
	{
		std::optional<math::point3f> get_face_plane_intersection(math::ray const& mouse_ray, egfx::face::cref face, math::transform_matrix const& t)
		{
			math::plane const local_plane = face.get_plane();
			math::plane const world_plane = transform(local_plane, t);
			return  mouse_ray.intersects(world_plane);
		}

		egfx::half_edge::id detect_hovered_edge(math::ray const& mouse_ray, egfx::face::cref face, math::transform_matrix const& t)
		{
			auto const intersection_result = get_face_plane_intersection(mouse_ray, face, t);
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
		, egfx::object::camera_cref main_camera
		, egfx::window const& main_window
		, entity_id selected_brush
		, egfx::face::id selected_face)
		: current_map(&current_map)
		, main_camera(main_camera)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
	{

	}

	brush const& face_context::get_brush() const
	{
		return *current_map->find_brush(selected_brush);
	}

	void face_context::update(action::accumulator& acc, input::frame_input& input)
	{
		hovered_edge = egfx::half_edge::id::none;

		brush const& b = get_brush();
		math::transform_matrix const t = b.get_world_transform(current_map->get_brush_root_world_transform());
		egfx::face::cref const current_face = b.mesh_def->get_face(selected_face);

		if (next_context == nullptr)
		{
			// Transparent overlay
			egfx::im::draw_face(current_face, t, egfx::color{ 1.0f, 1.0f, 1.0f, 0.6f });

			// Operation window
			operation_window();

			// Operation-specific logic
			if (current_operation == operation_type::edge_selection)
			{
				// Find the hovered edge
				if (has_focus(*main_window))
				{
					math::ray const mouse_ray = get_mouse_ray(*main_window, main_camera);
					hovered_edge = detect_hovered_edge(mouse_ray, current_face, t);
				}

				if (hovered_edge != egfx::half_edge::id::none)
				{
					if (input.consume_left_click())
					{
						next_context.reset(new edge_context(*current_map, main_camera, *main_window, selected_brush, selected_face, hovered_edge));
					}
					else
					{
						math::line const local_line = b.mesh_def->get_half_edge(hovered_edge).get_line();
						math::line const world_line = transform(local_line, t);
						Im3d::DrawLine(world_line.a, world_line.b, 5.f, Im3d::Color_Red);
					}
				}
			}
			else if(current_operation == operation_type::split)
			{
				if (has_focus(*main_window))
				{
					math::ray const mouse_ray = get_mouse_ray(*main_window, main_camera);
					auto const intersection_result = get_face_plane_intersection(mouse_ray, current_face, t);
					if (intersection_result)
					{
						math::point3f const local_intersection = transform(*intersection_result, invert(t));
						if (current_face.is_on_face(local_intersection))
						{
							Im3d::DrawPoint(*intersection_result, 10.f, Im3d::Color_Red);

							if (input.consume_left_click())
							{
								next_context.reset(new face_split_context(*current_map, *main_window, selected_brush, selected_face));
							}
						}
					}
				}
			}			
		}	
		else
		{
			if (input.consume_right_click())
			{
				next_context.reset();
			}

			composite_context::update(acc, input);
		}		
	}

	void face_context::operation_window()
	{
		brush const& b = get_brush();

		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
		ImGuiWindowFlags const flags = ImGuiWindowFlags_NoNav;
		auto const id_value = static_cast<std::underlying_type_t<entity_id>>(b.get_id());
		auto const face_value = static_cast<size_t>(selected_face);
		if (!ImGui::Begin(fmt::format("Brush {} face {}", id_value, face_value).c_str(), nullptr, flags))
		{
			ImGui::End();
			return;
		}

		if (ImGui::RadioButton("(E)dge Selection", current_operation == operation_type::edge_selection)) current_operation = operation_type::edge_selection;
		if (ImGui::RadioButton("(S)plit", current_operation == operation_type::split)) current_operation = operation_type::split;

		ImGui::End();
		ImGui::PopStyleColor();
	}

	bool face_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (composite_context::handle_keyboard_event(key, acc))
			return true;

		if (key.state == SDL_PRESSED && key.repeat == 0)
		{
			switch (key.keysym.sym)
			{
			case SDLK_e: current_operation = operation_type::edge_selection; return true;
			case SDLK_s: current_operation = operation_type::split; return true;
			}
		}

		return false;
	}
}
