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

			return ImGuizmo::Manipulate(view.elements, projection.elements, operation, ImGuizmo::LOCAL, object_transform.elements);
		}
	}

	brush_context::brush_context(map const& current_map, egfx::scene const& current_scene, egfx::window const& main_window, entity_id selected_brush) noexcept
		: current_map(&current_map)
		, current_scene(&current_scene)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, object_matrix(to_imgui(current_map.find_brush(selected_brush)->get_world_transform(math::transform_matrix::identity())))
	{
		
	}

	void brush_context::update(egfx::node::manual& m, action::accumulator& acc, input::frame_input& input)
	{
		hovered_face = egfx::face::id::none;

		egfx::object::camera_cref const camera = current_scene->get_camera();

		brush const& b = get_brush();
		egfx::mesh_definition const& mesh_def = *b.mesh_def;
		math::transform_matrix const t = b.get_world_transform(math::transform_matrix::identity());

		if (next_context == nullptr)
		{
			// Ensure the base transform stays up-to-date
			if (!is_editing)
				object_matrix = to_imgui(t);

			operation_window(acc);

			if (operation == operation_type::face_selection)
			{
				if (has_focus(*main_window) && !imgui::has_mouse())
				{
					hovered_face = get_closest_face(camera.get_position(), get_mouse_ray(*main_window, camera), t, mesh_def);
				}
			}
			else
			{
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
							acc.emplace_action<action::set_brush_position>(b, destination_from_origin(object_matrix.get_displacement()));
							break;
						case operation_type::rotation:
							acc.emplace_action<action::set_brush_rotation>(b, object_matrix.get_rotation());
							break;
						case operation_type::scale:
							acc.emplace_action<action::set_brush_scale>(b, object_matrix.get_scale());
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

	void brush_context::operation_window(action::accumulator& acc)
	{
		brush const& b = get_brush();

		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
		ImGuiWindowFlags const flags = ImGuiWindowFlags_NoNav;
		std::string const text = "Brush " + std::to_string(static_cast<std::underlying_type_t<entity_id>>(b.get_id()));
		if (!ImGui::Begin(text.c_str(), nullptr, flags))
		{
			ImGui::End();
			return;
		}			

		float translation[3], rotation[3], scale[3];
		ImGuizmo::DecomposeMatrixToComponents(object_matrix.elements, translation, rotation, scale);

		if (ImGui::RadioButton("(T)ranslate", operation == operation_type::translate)) operation = operation_type::translate;
		ImGui::SameLine();
		if (ImGui::InputFloat3("", translation))
		{
			acc.emplace_action<action::set_brush_position>(b, math::point3f{ translation[0], translation[1], translation[2] });
		}

		if (ImGui::RadioButton("(R)otation ", operation == operation_type::rotation)) operation = operation_type::rotation;
		ImGui::SameLine();
		if (ImGui::InputFloat3("", rotation))
		{
			acc.emplace_action<action::set_brush_position>(b, math::point3f{ rotation[0], rotation[1], rotation[2] });
		}

		if (ImGui::RadioButton("(S)cale    ", operation == operation_type::scale)) operation = operation_type::scale;
		ImGui::SameLine();
		if (ImGui::InputFloat3("", scale))
		{
			acc.emplace_action<action::set_brush_position>(b, math::point3f{ scale[0], scale[1], scale[2] });
		}

		if (ImGui::RadioButton("(F)ace Selection", operation == operation_type::face_selection)) operation = operation_type::face_selection;

		ImGui::End();
		ImGui::PopStyleColor();

		ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, object_matrix.elements);
	}

	brush const& brush_context::get_brush() const
	{
		return *current_map->find_brush(selected_brush);
	}

	bool brush_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (composite_context::handle_keyboard_event(key, acc))
			return true;

		if (key.state == SDL_PRESSED)
		{
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
}
