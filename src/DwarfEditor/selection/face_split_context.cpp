#include "face_split_context.h"

#include "action/brush.h"

#include "egfx/immediate.h"

#include <format>
#include <numbers>

#include <im3d.h>

namespace ot::dedit::selection
{
	namespace
	{
		math::quaternion get_rotation(math::plane const p)
		{
			math::plane const base{ {1,0,0}, 0 };

			float const d = dot_product(base.normal, p.normal);
			if (float_eq(d, 1.f))
			{
				return math::quaternion::identity();
			}
			else if (float_eq(d, -1.f))
			{
				return math::quaternion::y_rad_rotation(std::numbers::pi_v<float>);
			}
			else
			{
				math::vector3f const a = cross_product(base.normal, p.normal);
				float const s = std::sqrt((1.f + d) * 2.f);
				float const i = 1.f / s;
				return normalized(math::quaternion{ 
					s / 2.f
					, a.x * i, a.y * i, a.z * i 
				});
			}
		}

		math::transform_matrix get_current_plane_math_transform(math::transform_matrix const& brush_transform, brush const& b, egfx::face::id selected_face)
		{
			egfx::face::cref const f = b.get_mesh_def().get_face(selected_face);
			math::plane const p = f.get_plane();

			return math::transform_matrix::from_components(
				vector_from_origin(transform(p.get_point(), brush_transform))
				, get_rotation(p)
			);
		}

		imgui::matrix get_current_plane_imgui_transform(math::transform_matrix const& brush_transform, brush const& b, egfx::face::id selected_face)
		{
			egfx::face::cref const f = b.get_mesh_def().get_face(selected_face);
			math::plane const p = f.get_plane();

			return imgui::matrix::from_components(
				vector_from_origin(transform(p.get_point(), brush_transform))
				, get_rotation(p)
			);
		}

		imgui::matrix get_current_plane_imgui_transform(map const& current_map, entity_id selected_brush, egfx::face::id selected_face)
		{
			brush const& b = *current_map.find_brush(selected_brush);
			math::transform_matrix const t = b.get_world_transform();
			return get_current_plane_imgui_transform(t, b, selected_face);
		}
	}

	face_split_context::face_split_context(map const& current_map,
		egfx::window const& main_window
		, entity_id selected_brush
		, egfx::face::id selected_face
	)
		: current_map(&current_map)
		, main_window(&main_window)
		, selected_brush(selected_brush)
		, selected_face(selected_face)
		, plane_transform(imgui::matrix::identity())
		, current_operation(Im3d::GizmoMode_Translation)
	{
		
	}

	brush const& face_split_context::get_brush() const
	{
		return *current_map->find_brush(selected_brush);
	}

	void face_split_context::update(action::accumulator& acc, input::frame_input& input)
	{
		(void)acc;
		(void)input;

		operation_window(acc);

		Im3d::Context& im3d = Im3d::GetContext();
		im3d.m_gizmoMode = static_cast<Im3d::GizmoMode>(current_operation);
		im3d.m_gizmoLocal = true;

		imgui::matrix const face_world_transform = get_current_plane_imgui_transform(*current_map, selected_brush, selected_face);
		imgui::matrix plane_world_transform = plane_transform * face_world_transform;
		if (Im3d::Gizmo("SplitGizmo", plane_world_transform.elements))
		{
			plane_transform = plane_world_transform * invert(face_world_transform);
		}

		math::plane const base_plane{ {1,0,0}, 0 };
		math::plane const world_plane = transform(base_plane, to_math_matrix(plane_world_transform));

		math::vector3f const world_plane_displacement = plane_world_transform.get_displacement();

		Im3d::PushAlpha(0.5f);
		Im3d::PushColor(Im3d::Color_Blue);
		Im3d::DrawQuadFilled(world_plane_displacement, world_plane.normal, Im3d::Vec2(1.f, 1.f));
		Im3d::PopColor();
		Im3d::PopAlpha();

		Im3d::DrawArrow(world_plane_displacement, world_plane_displacement + world_plane.normal, 0.1f, 25.f);		
	}

	void face_split_context::operation_window(action::accumulator& acc)
	{
		brush const& b = get_brush();

		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
		ImGuiWindowFlags const flags = ImGuiWindowFlags_NoNav;
		auto const id_value = static_cast<std::underlying_type_t<entity_id>>(b.get_id());
		auto const face_value = static_cast<size_t>(selected_face);
		if (!ImGui::Begin(std::format("Brush {} face {}", id_value, face_value).c_str(), nullptr, flags))
		{
			ImGui::End();
			return;
		}

		if (ImGui::RadioButton("(T)ranslate", current_operation == Im3d::GizmoMode_Translation)) current_operation = Im3d::GizmoMode_Translation;
		if (ImGui::RadioButton("(R)otate", current_operation == Im3d::GizmoMode_Rotation)) current_operation = Im3d::GizmoMode_Rotation;
		if (ImGui::Button("(S)plit")) do_split(acc);

		ImGui::End();
		ImGui::PopStyleColor();
	}

	bool face_split_context::handle_keyboard_event(SDL_KeyboardEvent const& key, action::accumulator& acc)
	{
		(void)acc;

		if (key.state == SDL_PRESSED && key.repeat == 0)
		{
			switch (key.keysym.sym)
			{
			case SDLK_t: current_operation = Im3d::GizmoMode_Translation; return true;
			case SDLK_r: current_operation = Im3d::GizmoMode_Rotation; return true;
			case SDLK_s: do_split(acc); return true;
			}
		}

		return false;
	}

	void face_split_context::do_split(action::accumulator& acc)
	{
		brush const& b = get_brush();
		math::transform_matrix const t = b.get_world_transform();

		math::transform_matrix const face_world_transform = get_current_plane_math_transform(t, b, selected_face);
		math::transform_matrix const plane_world_transform = to_math_matrix(plane_transform) * face_world_transform;
		math::plane const base_plane{ {1,0,0}, 0 };
		math::plane const world_plane = transform(base_plane, plane_world_transform);
		math::plane const local_plane = transform(world_plane, invert(t));
		acc.emplace_action<action::split_brush_face>(b, selected_face, local_plane);
	}
}
