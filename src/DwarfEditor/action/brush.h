#pragma once

#include "action/brush.fwd.h"
#include "egfx/mesh_definition.fwd.h"
#include "map.h"

#include <memory>

namespace ot::dedit::action
{
	class brush_base
	{
	public:
		virtual ~brush_base();

		virtual void apply(map& current_map) = 0;
		virtual void undo(map& current_map) = 0;
	};

	class single_brush : public brush_base
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

	class split_edge : public brush_definition_base
	{
		egfx::half_edge::id edge;
		math::point3f point;

	protected:
		virtual void do_apply(brush& b) override;

	public:
		split_edge(brush const& b, egfx::half_edge::id edge, math::point3f point);
	};

	class set_position : public single_brush
	{
		math::point3f previous_state;
		math::point3f new_pos;

	protected:
		virtual void do_apply(brush& b) override;
		virtual void do_undo(brush& b) override;

	public:
		set_position(brush const& b, math::point3f point);
	};

	class set_rotation : public single_brush
	{
		math::quaternion previous_state;
		math::quaternion new_rot;
	
	protected:
		virtual void do_apply(brush& b) override;
		virtual void do_undo(brush& b) override;

	public:
		set_rotation(brush const& b, math::quaternion rot);
	};

	class set_scale : public single_brush
	{
		float previous_state;
		float new_s;

	protected:
		virtual void do_apply(brush& b) override;
		virtual void do_undo(brush& b) override;

	public:
		set_scale(brush const& b, float s);
	};
}
