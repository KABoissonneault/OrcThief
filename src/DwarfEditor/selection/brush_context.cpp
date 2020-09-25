#include "selection/brush_context.h"

#include "selection/face_context.h"
#include "selection/brush_common.h"

#include "action/brush.h"

#include "datablock.h"
#include "input.h"
#include "imgui/projection.h"
#include "imgui/module.h"

#include "egfx/object/camera.h"
#include "egfx/window.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace ot::dedit::selection
{
	namespace
	{
		egfx::face::id get_closest_face(math::point3f camera_wpos, math::ray const& mouse_ray, math::transform_matrix const& brush_transform, egfx::mesh_definition const& mesh)
		{
			egfx::face::id current_face = egfx::face::id::none;
			float current_distance_sq = FLT_MAX;
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
						float const intersection_distance_sq = (camera_wpos - intersection).norm_squared();
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

		void add_manual_scene(egfx::node::manual& m, egfx::mesh_definition const& mesh_def, math::transform_matrix const& t, egfx::face::id hovered_face)
		{
			m.add_wiremesh(datablock::overlay_unlit, mesh_def, t);

			if (hovered_face != egfx::face::id::none)
			{
				m.add_face(datablock::overlay_unlit_transparent_light, mesh_def.get_face(hovered_face), t);
			}

			for (egfx::vertex::cref const vertex : mesh_def.get_vertices())
			{
				math::point3f const vertex_pos = transform(vertex.get_position(), t);
				auto const vt = math::transform_matrix::from_components(vector_from_origin(vertex_pos), math::quaternion::identity(), 0.04f);
				m.add_mesh(datablock::overlay_unlit_vertex, egfx::mesh_definition::get_cube(), vt);
			}
		}

		bool add_guizmo(egfx::object::camera_cref camera, imgui::matrix& object_transform, ImGuizmo::OPERATION operation)
		{
			imgui::matrix const view = to_imgui(camera.get_view_matrix());
			imgui::matrix const projection = imgui::make_perspective_projection(camera.get_rad_fov_y(), camera.get_aspect_ratio(), camera.get_z_near(), camera.get_z_far());

			{
				imgui::matrix const grid_transform = imgui::matrix::identity();
				ImGuizmo::DrawGrid(view.elements, projection.elements, grid_transform.elements, 10.f);
			}

			return ImGuizmo::Manipulate(view.elements, projection.elements, operation, ImGuizmo::LOCAL, object_transform.elements);
		}
	}

	brush_context::brush_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window, size_t selected_brush) noexcept
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, object_matrix(to_imgui(current_map.get_brush(selected_brush).get_world_transform(math::transform_matrix::identity())))
	{
		select(selected_brush);
	}

	void brush_context::update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input)
	{
		hovered_face = egfx::face::id::none;

		egfx::object::camera_cref const camera = current_scene->get_camera();

		brush const& b = current_map->get_brushes()[selected_brush];
		egfx::mesh_definition const& mesh_def = *b.mesh_def;
		math::transform_matrix const t = b.get_world_transform(math::transform_matrix::identity());

		if (next_context == nullptr)
		{
			operation_window();

			if (operation == operation_type::face_selection)
			{
				if (has_focus(*main_window) && !imgui::has_mouse())
				{
					hovered_face = get_closest_face(camera.get_position(), get_mouse_ray(*main_window, camera), t, mesh_def);
				}
			}
			else
			{
				// Ensure the base transform stays up-to-date
				if (!is_editing)
					object_matrix = to_imgui(t);

				add_guizmo(camera, object_matrix, static_cast<ImGuizmo::OPERATION>(operation));
				
				// If we're using ImGuizmo, we're in the middle of editing
				if (ImGuizmo::IsUsing())
				{
					is_editing = true;
				}
				// If we just stopped using ImGuizmo, then we need to commit the edit if any
				else if (is_editing)
				{
					if (!float_eq(object_matrix, t))
					{
						switch (operation)
						{
						case operation_type::translate:
							acc.emplace_brush_action<action::set_position>(b, destination_from_origin(object_matrix.get_displacement()));
							break;
						case operation_type::rotation:
							acc.emplace_brush_action<action::set_rotation>(b, object_matrix.get_rotation());
							break;
						case operation_type::scale:
							acc.emplace_brush_action<action::set_scale>(b, object_matrix.get_scale());
							break;
						}
					}
					is_editing = false;
				}
			}
		}	

		add_manual_scene(m, mesh_def, t, hovered_face);

		composite_context::update(m, acc, input);

		if (next_context == nullptr)
		{
			if (hovered_face != egfx::face::id::none && input.consume_left_click())
			{
				next_context.reset(new face_context(*current_map, *current_scene, *main_window, selected_brush, hovered_face));
			}
		}
		else if(input.consume_right_click())
		{
			next_context.reset();
		}
	}

	void brush_context::operation_window()
	{
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
		ImGuiWindowFlags const flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Brush Context", nullptr, flags);

		if (ImGui::RadioButton("(T)ranslate", operation == operation_type::translate)) operation = operation_type::translate;
		if (ImGui::RadioButton("(R)otation", operation == operation_type::rotation)) operation = operation_type::rotation;
		if (ImGui::RadioButton("(S)cale", operation == operation_type::scale)) operation = operation_type::scale;
		if (ImGui::RadioButton("(F)ace Selection", operation == operation_type::face_selection)) operation = operation_type::face_selection;

		ImGui::End();
		ImGui::PopStyleColor();
	}

	bool brush_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (composite_context::handle_keyboard_event(key, acc))
			return true;

		SDL_Keysym const& keysym = key.keysym;
		if (key.state == SDL_PRESSED)
		{
			switch (key.keysym.scancode)
			{
			case SDL_SCANCODE_TAB:
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
			
			switch (key.keysym.sym)
			{
			case SDLK_t: operation = operation_type::translate; return true;
			case SDLK_r: operation = operation_type::rotation; return true;
			case SDLK_s: operation = operation_type::scale; return true;
			case SDLK_f: operation = operation_type::face_selection; return true;
			}
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
		}
	}
}
