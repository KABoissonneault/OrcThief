#include "selection/node_context.h"

#include "selection/face_context.h"
#include "selection/brush_common.h"

#include "action/brush.h"
#include "action/light.h"

#include "datablock.h"
#include "input.h"
#include "imgui/projection.h"
#include "imgui/module.h"

#include "egfx/object/camera.h"
#include "egfx/window.h"
#include "egfx/immediate.h"

#include <imgui.h>
#include <im3d.h>

#include <format>
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

		void draw_operation_preview(map_entity const& e, imgui::matrix const& m)
		{
			if (e.get_type() == entity_type::brush)
			{
				brush_entity const& b = static_cast<brush_entity const&>(e);
				egfx::im::draw_mesh(b.get_mesh_def(), to_math_matrix(m), egfx::color{1.f, 1.f, 1.f, 0.2f});
			}
		}
	}

	node_context::node_context(map const& current_map, egfx::camera_cref main_camera, egfx::window const& main_window, entity_id selected_node) noexcept
		: current_map(&current_map)
		, main_camera(main_camera)
		, main_window(&main_window)
		, selected_node(selected_node)
		, object_local_matrix(to_imgui(current_map.find_entity(selected_node)->get_local_transform()))
	{

	}

	void node_context::update(action::accumulator& acc, input::frame_input& input)
	{
		map_entity const& e = get_entity();

		bool const is_brush = e.get_type() == entity_type::brush;

		hovered_face = egfx::face::id::none;

		brush_entity const* b = is_brush ? &static_cast<brush_entity const&>(e) : nullptr;
		map_entity const* const parent_entity = e.get_parent();
		math::transform_matrix const parent_world_transform = parent_entity != nullptr ? parent_entity->get_world_transform() : math::transform_matrix::identity();
		math::transform_matrix const local_transform = e.get_local_transform();
		math::transform_matrix const world_transform = parent_world_transform * local_transform;

		if (next_context == nullptr)
		{
			// Ensure the base transform stays up-to-date
			if (!was_editing)
				object_local_matrix = to_imgui(local_transform);
						
			bool const text_editing = operation_window(acc, local_transform);
			imgui::matrix object_world_matrix = parent_world_transform * object_local_matrix;

			if (is_brush && operation == operation_type::face_selection)
			{
				if (has_focus(*main_window) && !imgui::has_mouse())
				{
					hovered_face = get_closest_face(main_camera.get_position(), get_mouse_ray(*main_window, main_camera), world_transform, b->get_mesh_def());
				}

				if (text_editing)
				{
					draw_operation_preview(e, object_world_matrix);
				}
								
				was_editing = text_editing;
			}
			else
			{
				bool const gizmo_editing = draw_gizmo(object_world_matrix);
				if (gizmo_editing || text_editing)
				{
					draw_operation_preview(e, object_world_matrix);
				}

				if (gizmo_editing)
				{
					object_local_matrix = object_world_matrix * invert(parent_world_transform);
					was_gizmo_editing = true;
				}
				// If we just stopped using the gizmo, then we need to commit the edit if any
				else if (was_gizmo_editing)
				{
					if (!float_eq(object_local_matrix, local_transform))
					{
						math::transform_matrix const new_local_transform = to_math_matrix(object_local_matrix);

						switch (operation)
						{
						case operation_type::translate:
							math::point3f const new_position = destination_from_origin(new_local_transform.get_displacement());
							acc.emplace_action<action::set_entity_position>(e, new_position);
							break;
						case operation_type::rotation:
							acc.emplace_action<action::set_entity_rotation>(e, new_local_transform.get_rotation().to_quaternion());
							break;
						case operation_type::scale:
							acc.emplace_action<action::set_entity_scale>(e, new_local_transform.get_scale());
							break;
						}
					}
					was_gizmo_editing = false;
				}

				was_editing = text_editing || gizmo_editing;
			}

			was_text_editing = text_editing;

			properties_window(acc);
		}

		draw_immediate_scene(world_transform);

		composite_context::update(acc, input);

		if (next_context == nullptr)
		{
			if (hovered_face != egfx::face::id::none && input.consume_left_click())
			{
				next_context.reset(new face_context(*current_map, main_camera, *main_window, selected_node, hovered_face));
			}
		}
		else if (input.consume_right_click())
		{
			next_context.reset();
		}
	}

	bool node_context::operation_window(action::accumulator& acc, math::transform_matrix const& original_local_transform)
	{
		map_entity const& e = get_entity();
		bool const is_brush = e.get_type() == entity_type::brush;

		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
		ImGuiWindowFlags const flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing;
		if (!ImGui::Begin(std::format("Operations", e.get_name()).c_str(), nullptr, flags))
		{
			ImGui::End();
			ImGui::PopStyleColor();
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
		object_local_matrix.decompose(translation, rotation, scale);
		rotation[0] *= 180.f / std::numbers::pi_v<float>;
		rotation[1] *= 180.f / std::numbers::pi_v<float>;
		rotation[2] *= 180.f / std::numbers::pi_v<float>;

		rotation[0] += 0.0f;
		rotation[1] += 0.0f;
		rotation[2] += 0.0f;

		math::euler_rotation const original_euler_rotation_rad = original_local_transform.get_rotation().get_euler_angles_zyx();
		math::vector3f const original_euler_rotation_deg = math::vector3f{ original_euler_rotation_rad.x,  original_euler_rotation_rad.y, original_euler_rotation_rad.z } * 180.f / std::numbers::pi_v<float>;
		math::scales const original_scale = original_local_transform.get_scale();
		
		if (ImGui::RadioButton("(T)ranslate", operation == operation_type::translate)) operation = operation_type::translate;
		ImGui::SameLine();
		ImGui::InputFloat3("##NodeTranslate", translation);
		bool const translate_active = ImGui::IsItemActive();
		bool const has_translated = !translate_active && !float_eq(math::vector3f{ translation[0], translation[1], translation[2] }, original_local_transform.get_displacement());

		if (ImGui::RadioButton("(R)otate   ", operation == operation_type::rotation)) operation = operation_type::rotation;
		ImGui::SameLine();
		ImGui::InputFloat3("##NodeRotation", rotation);
		bool const rotation_active = ImGui::IsItemActive();
		// Use more bits of imprecision, rotations are pretty approximate
		bool const has_rotated = !rotation_active && !float_eq(math::vector3f{ rotation[0], rotation[1], rotation[2] }, original_euler_rotation_deg, 2);

		if (ImGui::RadioButton("(S)cale    ", operation == operation_type::scale)) operation = operation_type::scale;
		ImGui::SameLine();
		ImGui::InputFloat3("##NodeScale", scale);
		bool const scale_active = ImGui::IsItemActive();
		bool const has_scaled = !scale_active && !float_eq(math::vector3f{ scale[0], scale[1], scale[2] }, math::vector3f{ original_scale.x, original_scale.y, original_scale.z });

		if (is_brush && ImGui::RadioButton("(F)ace Selection", operation == operation_type::face_selection)) operation = operation_type::face_selection;

		ImGui::End();
		ImGui::PopStyleColor();

		rotation[0] *= std::numbers::pi_v<float> / 180.f;
		rotation[1] *= std::numbers::pi_v<float> / 180.f;
		rotation[2] *= std::numbers::pi_v<float> / 180.f;
		
		object_local_matrix.recompose(translation, rotation, scale);
				
		if (was_text_editing)
		{
			if (has_translated)
			{
				acc.emplace_action<action::set_entity_position>(e, math::point3f{ translation[0], translation[1], translation[2] });
			}

			if (has_rotated)
			{
				auto clamp = [](float f)
				{
					return std::fmod(f + std::numbers::pi_v<float>, std::numbers::pi_v<float> *2.f) - std::numbers::pi_v<float>;
				};

				math::euler_rotation const euler_angles
				{
					clamp(rotation[0])
					, clamp(rotation[1])
					, clamp(rotation[2])
				};

				math::rotation_matrix const new_rotation = math::rotation_matrix::rot_zyx(euler_angles);

				acc.emplace_action<action::set_entity_rotation>(e, new_rotation.to_quaternion());
			}

			if (has_scaled)
			{
				acc.emplace_action<action::set_entity_scale>(e, math::scales{ scale[0], scale[1], scale[2] });
			}
		}

		return translate_active || rotation_active || scale_active;
	}

	void node_context::properties_window(action::accumulator& acc)
	{
		map_entity const& e = get_entity();

		if (ImGui::Begin("Properties"))
		{
			std::string_view const name = e.get_name();
			ImGui::Text("%.*s", name.size(), name.data());
						
			egfx::node_cref node = e.get_node();
			egfx::object_cref main_object = node.get_object(0);

			ImGui::Text("Object %u (%s)", as_int(main_object.get_object_id()), as_string(main_object.get_object_type()).data());

			bool is_casting_shadows = main_object.is_casting_shadows();
			ImGui::Checkbox("Casts Shadows", &is_casting_shadows);
			if (ImGui::IsItemDeactivatedAfterEdit())
				acc.emplace_action<action::set_object_casts_shadows>(main_object, is_casting_shadows);

			switch (e.get_type())
			{
			case entity_type::brush:
				brush_properties(static_cast<brush_entity const&>(e), acc);
				break;

			case entity_type::light:
				light_properties(static_cast<light_entity const&>(e), acc);
				break;
			}			
		}
		ImGui::End();
	}

	void node_context::brush_properties(brush_entity const& b, action::accumulator& acc)
	{
		if (ImGui::BeginMenu("Vertex Debug"))
		{
			if (ImGui::MenuItem("None", nullptr, vertex_debug == vertex_debug_type::none))
				vertex_debug = vertex_debug_type::none;
			if (ImGui::MenuItem("UV", nullptr, vertex_debug == vertex_debug_type::uv))
				vertex_debug = vertex_debug_type::uv;

			ImGui::EndMenu();
		}

		egfx::material_handle_t const mesh_material = b.get_item().get_material();
		std::string const current_material_name = mesh_material.is_null() ? "" : egfx::get_material_name(mesh_material);
		if (ImGui::BeginCombo("Material", current_material_name.c_str()))
		{
			if (cached_materials.empty())
			{
				cached_materials = egfx::get_registered_materials();
			}

			for (egfx::material_handle_t const& mat : cached_materials)
			{
				std::string const mat_name = egfx::get_material_name(mat);

				bool const selected = mat == mesh_material;
				if (ImGui::Selectable(mat_name.c_str(), selected))
				{
					acc.emplace_action<action::set_brush_material>(b, mat);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		else
		{
			cached_materials.clear();
		}
	}

	void node_context::light_properties(light_entity const& l, action::accumulator& acc)
	{
		egfx::light_cref const light = l.get_light();
		egfx::light_type const current_type = light.get_light_type();
		if (ImGui::BeginCombo("Light Type", as_string(current_type).data()))
		{
			auto combo_item = [current_type, &light, &acc] (egfx::light_type const type)
			{
				bool const selected = type == current_type;
				if (ImGui::Selectable(as_string(type).data(), selected))
				{
					acc.emplace_action<action::set_light_type>(light, type);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			};

			combo_item(egfx::light_type::directional);
			combo_item(egfx::light_type::point);
			combo_item(egfx::light_type::spotlight);

			ImGui::EndCombo();
		}

		float power_scale = light.get_power_scale();
		ImGui::InputFloat("Power Scale", &power_scale);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			acc.emplace_action<action::set_light_power_scale>(light, power_scale);
		}

		egfx::color color = light.get_diffuse();
		ImGui::ColorEdit3("Diffuse", &color.r);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			acc.emplace_action<action::set_light_diffuse>(light, color);
		}

		float attenuation_range = light.get_attenuation_range();
		float attenuation_const = light.get_attenuation_const();
		float attenuation_linear = light.get_attenuation_linear();
		float attenuation_quadratic = light.get_attenuation_quadratic();
		ImGui::Text("Attenuation");
		ImGui::Indent();
		{
			bool deactivated_after_edit = false;
			ImGui::InputFloat("Range", &attenuation_range);
			deactivated_after_edit |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::InputFloat("Const", &attenuation_const);
			deactivated_after_edit |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::InputFloat("Linear", &attenuation_linear);
			deactivated_after_edit |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::InputFloat("Quadratic", &attenuation_quadratic);
			deactivated_after_edit |= ImGui::IsItemDeactivatedAfterEdit();

			if (deactivated_after_edit)
				acc.emplace_action<action::set_light_attenuation>(light, attenuation_range, attenuation_const, attenuation_linear, attenuation_quadratic);
		}
		ImGui::Unindent();
	}

	bool node_context::draw_gizmo(imgui::matrix& object_world_matrix)
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

		return Im3d::Gizmo("BrushGizmo", object_world_matrix.elements);
	}

	map_entity const& node_context::get_entity() const
	{
		return *current_map->find_entity(selected_node);
	}

	bool node_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		if (composite_context::handle_keyboard_event(key, acc))
			return true;

		map_entity const& e = get_entity();
		bool const is_brush = e.get_type() == entity_type::brush;

		if (key.state == SDL_PRESSED && key.repeat == 0 && !ImGui::GetIO().WantTextInput)
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
			case SDLK_f: 
				if (is_brush)
				{
					operation = operation_type::face_selection;
					return true;
				}

			case SDLK_DELETE:
			{
				acc.emplace_action<action::delete_entity>(e);
				return true;
			}
			}			
		}

		return false;
	}

	void node_context::draw_immediate_scene(math::transform_matrix const& t)
	{
		map_entity const& e = get_entity();
		if (e.get_type() == entity_type::brush)
		{
			brush_entity const& b = static_cast<brush_entity const&>(e);
			egfx::mesh_definition const& mesh_def = b.get_mesh_def();
			egfx::im::draw_wiremesh(mesh_def, t, 2.f);

			if (hovered_face != egfx::face::id::none)
			{
				egfx::im::draw_face(mesh_def.get_face(hovered_face), t, egfx::color{ 1.f, 1.f, 1.f, 0.2f });
			}

			for (egfx::vertex::cref const vertex : mesh_def.get_vertices())
			{
				math::point3f const vertex_pos = transform(vertex.get_position(), t);
				Im3d::DrawPoint(vertex_pos, 10.f, Im3d::Color_Blue);

				switch (vertex_debug)
				{
				case vertex_debug_type::uv:
					math::point2f const uv = vertex.get_uv();
					Im3d::Text(vertex_pos, 2.f, Im3d::Color_White, Im3d::TextFlags_Default, "%.3f, %.3f", uv.x, uv.y);
					break;
				}
			}
		}
	}
}
