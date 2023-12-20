#pragma once

#include "action/base.h"
#include "egfx/mesh_definition.fwd.h"
#include "map.h"

#include <memory>

namespace ot::dedit::action
{
	class spawn_brush : public base
	{
		std::shared_ptr<egfx::mesh_definition const> mesh_def;
		entity_id id;

	public:
		spawn_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id);

		virtual void apply(map& current_map) override;
		virtual void undo(map& current_map) override;
	};

	class single_brush : public base
	{
		entity_id id;

	protected:
		single_brush(brush const& b);

		entity_id get_id() const noexcept { return id; }

		virtual void do_apply(brush& b) = 0;
		virtual void do_undo(brush& b) = 0;

	public:
		virtual void apply(map& current_map) override final;
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
		virtual void do_apply(brush& b) override;

	public:
		split_brush_edge(brush const& b, egfx::half_edge::id edge, math::point3f point);
	};

	class split_brush_face : public brush_definition_base
	{
		egfx::face::id face;
		math::plane plane;

	protected:
		virtual void do_apply(brush& b) override;

	public:
		split_brush_face(brush const& b, egfx::face::id face, math::plane plane);
	};

	class set_brush_position : public single_brush
	{
		math::point3f previous_state;
		math::point3f new_pos;

	protected:
		virtual void do_apply(brush& b) override;
		virtual void do_undo(brush& b) override;

	public:
		set_brush_position(brush const& b, math::point3f point);
	};

	class set_brush_rotation : public single_brush
	{
		math::quaternion previous_state;
		math::quaternion new_rot;
	
	protected:
		virtual void do_apply(brush& b) override;
		virtual void do_undo(brush& b) override;

	public:
		set_brush_rotation(brush const& b, math::quaternion rot);
	};

	class set_brush_scale : public single_brush
	{
		math::scales previous_state;
		math::scales new_s;

	protected:
		virtual void do_apply(brush& b) override;
		virtual void do_undo(brush& b) override;

	public:
		set_brush_scale(brush const& b, math::scales s);
	};

	class delete_brush : public base
	{
		entity_id id;
		math::transform_matrix previous_transform;
		std::shared_ptr<egfx::mesh_definition const> previous_mesh_def;

	protected:
		virtual void apply(map& current_map) override;
		virtual void undo(map& current_map) override;

	public:
		delete_brush(brush const& b);
	};
}
