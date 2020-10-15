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
#include "egfx/immediate.h"

#include <imgui.h>
#include <im3d.h>

#include <numbers>

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

		void draw_operation_preview(egfx::mesh_definition const& mesh_def, imgui::matrix const& m)
		{
			egfx::im::draw_mesh(mesh_def, to_math_matrix(m), egfx::color{ 1.f, 1.f, 1.f, 0.2f });
		}

		void draw_immediate_scene(egfx::mesh_definition const& mesh_def, math::transform_matrix const& t, egfx::face::id hovered_face)
		{
			egfx::im::draw_wiremesh(mesh_def, t, 2.f);

			if (hovered_face != egfx::face::id::none)
			{
				egfx::im::draw_face(mesh_def.get_face(hovered_face), t, egfx::color{ 1.f, 1.f, 1.f, 0.2f });
			}

			for (egfx::vertex::cref const vertex : mesh_def.get_vertices())
			{
				math::point3f const vertex_pos = transform(vertex.get_position(), t);
				Im3d::DrawPoint(vertex_pos, 10.f, Im3d::Color_Blue);
			}
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

	void brush_context::update(action::accumulator& acc, input::frame_input& input)
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

			bool const text_editing = operation_window(acc);

			if (operation == operation_type::face_selection)
			{
				if (has_focus(*main_window) && !imgui::has_mouse())
				{
					hovered_face = get_closest_face(camera.get_position(), get_mouse_ray(*main_window, camera), t, mesh_def);
				}

				if (text_editing)
				{
					draw_operation_preview(mesh_def, object_matrix);
				}
			}
			else
			{
				bool const gizmo_editing = draw_gizmo();
				if (gizmo_editing || text_editing)
				{
					draw_operation_preview(mesh_def, object_matrix);
				}

				if (gizmo_editing)
				{
					is_editing = true;
				}
				// If we just stopped using the gizmo, then we need to commit the edit if any
				else if(is_editing)
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

		draw_immediate_scene(mesh_def, t, hovered_face);

		composite_context::update(acc, input);

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

	bool brush_context::operation_window(action::accumulator& acc)
	{
		brush const& b = get_brush();

		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
		ImGuiWindowFlags const flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;
		std::string const text = "Brush " + std::to_string(static_cast<std::underlying_type_t<entity_id>>(b.get_id()));
		if (!ImGui::Begin(text.c_str(), nullptr, flags))
		{
			ImGui::End();
			return false;
		}			

		if (!(operation == operation_type::translate || operation == operation_type::rotation))
		{
			ImGui::PushItemDisabled();
		}

		ImGui::Checkbox("(W)orld Manipulation", &use_world_manipulation);

		if (!(operation == operation_type::translate || operation == operation_type::rotation))
		{
			ImGui::PopItemDisabled();
		}

		float translation[3], rotation[3], scale[3];
		object_matrix.decompose(translation, rotation, scale);
		rotation[0] *= 180.f / std::numbers::pi_v<float>;
		rotation[1] *= 180.f / std::numbers::pi_v<float>;
		rotation[2] *= 180.f / std::numbers::pi_v<float>;

		math::vector3f const original_translation{ translation[0], translation[1], translation[2] };
		math::vector3f const original_rotation{ rotation[0], rotation[1], rotation[2] };
		math::vector3f const original_scale{ scale[0], scale[1], scale[2] };

		if (ImGui::RadioButton("(T)ranslate", operation == operation_type::translate)) operation = operation_type::translate;
		ImGui::SameLine();
		ImGui::InputFloat3("##BrushTranslate", translation);
		bool const translate_active = ImGui::IsItemActive();
		bool const has_translated = !translate_active && !float_eq(math::vector3f{ translation[0], translation[1], translation[2] }, original_translation);

		if (ImGui::RadioButton("(R)otation ", operation == operation_type::rotation)) operation = operation_type::rotation;
		ImGui::SameLine();
		ImGui::InputFloat3("##BrushRotation", rotation);
		bool const rotation_active = ImGui::IsItemActive();
		bool const has_rotated = !rotation_active && !float_eq(math::vector3f{ rotation[0], rotation[1], rotation[2] }, original_rotation);

		if (ImGui::RadioButton("(S)cale    ", operation == operation_type::scale)) operation = operation_type::scale;
		ImGui::SameLine();
		ImGui::InputFloat3("##BrushScale", scale);
		bool const scale_active = ImGui::IsItemActive();
		bool const has_scaled = !scale_active && !float_eq(math::vector3f{ scale[0], scale[1], scale[2] }, original_scale);

		if (ImGui::RadioButton("(F)ace Selection", operation == operation_type::face_selection)) operation = operation_type::face_selection;

		ImGui::End();
		ImGui::PopStyleColor();

		rotation[0] *= std::numbers::pi_v<float> / 180.f;
		rotation[1] *= std::numbers::pi_v<float> / 180.f;
		rotation[2] *= std::numbers::pi_v<float> / 180.f;
		object_matrix.recompose(translation, rotation, scale);
				
		if(has_translated) 
			acc.emplace_action<action::set_brush_position>(b, math::point3f{ translation[0], translation[1], translation[2] });
		if(has_rotated) 
			acc.emplace_action<action::set_brush_rotation>(b, object_matrix.get_rotation());
		if(has_scaled) 
			acc.emplace_action<action::set_brush_scale>(b, math::scales{ scale[0], scale[1], scale[2] });

		return translate_active || rotation_active || scale_active;
	}

	bool brush_context::draw_gizmo()
	{
		Im3d::Context& im3d = Im3d::GetContext();
		assert(operation != operation_type::face_selection);
		switch (operation)
		{
		case operation_type::translate: 
			im3d.m_gizmoMode = Im3d::GizmoMode_Translation;
			im3d.m_gizmoLocal = !use_world_manipulation;
			break;
		case operation_type::rotation:
			im3d.m_gizmoMode = Im3d::GizmoMode_Rotation;
			im3d.m_gizmoLocal = !use_world_manipulation;
			break;
		case operation_type::scale:
			im3d.m_gizmoMode = Im3d::GizmoMode_Scale;
			break;
		}
		return Im3d::Gizmo("BrushGizmo", object_matrix.elements);
	}

	brush const& brush_context::get_brush() const
	{
		return *current_map->find_brush(selected_brush);
	}

	bool brush_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (composite_context::handle_keyboard_event(key, acc))
			return true;

		if (key.state == SDL_PRESSED && key.repeat == 0)
		{
			switch (key.keysym.sym)
			{
			case SDLK_w: 
				if (operation == operation_type::translate || operation == operation_type::rotation)
				{
					use_world_manipulation = !use_world_manipulation;
				}
				return true;
			case SDLK_t: operation = operation_type::translate; return true;
			case SDLK_r: operation = operation_type::rotation; return true;
			case SDLK_s: operation = operation_type::scale; return true;
			case SDLK_f: operation = operation_type::face_selection; return true;
			}
		}

		return false;
	}
}
