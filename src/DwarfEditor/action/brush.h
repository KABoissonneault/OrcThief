#pragma once

#include "action/base.h"
#include "action/map_entity.h"
#include "egfx/mesh_definition.fwd.h"
#include "map.h"

#include <memory>
#include <optional>

namespace ot::dedit::action
{
	class single_brush : public base
	{
		entity_id id;

	protected:
		single_brush(brush const& b);

		entity_id get_id() const noexcept { return id; }

		virtual void do_apply(brush& b, bool is_redo) = 0;
		virtual void do_undo(brush& b) = 0;

	public:
		virtual void apply(map& current_map) override final;
		virtual void redo(map& current_map) override final;
		virtual void undo(map& current_map) override final;
	};

	// For actions that change the mesh's definition
	class brush_definition_base : public single_brush
	{		
		std::shared_ptr<egfx::mesh_definition const> previous_state;
	protected:
		brush_definition_base(brush const& b);

		virtual void do_undo(brush& b) override;
	};

	class split_brush_edge : public brush_definition_base
	{
		egfx::half_edge::id edge;
		math::point3f point;

	protected:
		virtual void do_apply(brush& b, bool is_redo) override;

	public:
		split_brush_edge(brush const& b, egfx::half_edge::id edge, math::point3f point);
	};

	class split_brush_face : public brush_definition_base
	{
		egfx::face::id face;
		math::plane plane;

	protected:
		virtual void do_apply(brush& b, bool is_redo) override;

	public:
		split_brush_face(brush const& b, egfx::face::id face, math::plane plane);
	};
		
	class set_brush_material : public single_brush
	{
		egfx::material_handle_t previous_material;
		egfx::material_handle_t new_material;
	protected:
		virtual void do_apply(brush& b, bool is_redo) override;
		virtual void do_undo(brush& b) override;

	public:
		set_brush_material(brush const& b, egfx::material_handle_t const& mat);
	};
}
