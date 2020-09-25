#include "action/brush.h"

#include "core/fwd_delete.h"

#include <cstdio>

namespace ot::dedit::action
{
	brush_base::~brush_base()
	{

	}

	single_brush::single_brush(brush const& b)
		: id(b.get_id())
	{

	}

	void single_brush::apply(map& current_map)
	{
		brush* b = current_map.find_brush(get_id());
		if (b == nullptr)
		{
			std::fprintf(stderr, "error: could not apply action, entity id '%u' not found\n", static_cast<uint32_t>(get_id()));
			return;
		}

		do_apply(*b);
	}

	void single_brush::undo(map& current_map)
	{
		brush* b = current_map.find_brush(get_id());
		if (b == nullptr)
		{
			std::fprintf(stderr, "error: could not undo action, entity id '%u' not found\n", static_cast<uint32_t>(get_id()));
			return;
		}

		do_undo(*b);
	}

	brush_definition_base::brush_definition_base(brush const& b)
		: single_brush(b)
		, previous_state(b.mesh_def)
	{

	}

	void brush_definition_base::do_undo(brush& b)
	{
		b.reload_node(std::move(previous_state));
	}

	split_edge::split_edge(brush const& b, egfx::half_edge::id edge, math::point3f point)
		: brush_definition_base(b)
		, edge(edge)
		, point(point)
	{

	}

	void split_edge::do_apply(brush& b)
	{
		auto new_mesh = std::make_shared<egfx::mesh_definition>(*b.mesh_def);
		new_mesh->get_half_edge(edge).split_at(point);
		b.reload_node(std::move(new_mesh));
	}

	set_position::set_position(brush const& b, math::point3f point)
		: single_brush(b)
		, previous_state(b.node.get_position())
		, new_pos(point)
	{

	}

	void set_position::do_apply(brush& b)
	{
		b.node.set_position(new_pos);
	}

	void set_position::do_undo(brush& b)
	{
		b.node.set_position(previous_state);
	}

	set_rotation::set_rotation(brush const& b, math::quaternion rot)
		: single_brush(b)
		, previous_state(b.node.get_rotation())
		, new_rot(rot)
	{

	}

	void set_rotation::do_apply(brush& b)
	{
		b.node.set_rotation(new_rot);
	}

	void set_rotation::do_undo(brush& b)
	{
		b.node.set_rotation(previous_state);
	}

	set_scale::set_scale(brush const& b, float s)
		: single_brush(b)
		, previous_state(b.node.get_scale())
		, new_s(s)
	{

	}

	void set_scale::do_apply(brush& b)
	{
		b.node.set_scale(new_s);
	}

	void set_scale::do_undo(brush& b)
	{
		b.node.set_scale(previous_state);
	}
}

template struct ot::fwd_delete<ot::dedit::action::brush_base>;
