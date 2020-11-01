#include "action/brush.h"

#include "console.h"
#include "egfx/mesh_definition.h"

#include <fmt/format.h>

namespace ot::dedit::action
{
	spawn_brush::spawn_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id)
		: mesh_def(std::move(mesh_def))
		, id(id)
	{

	}

	void spawn_brush::apply(map& current_map)
	{
		current_map.make_brush(mesh_def, id);
	}
	
	void spawn_brush::undo(map& current_map)
	{
		current_map.delete_brush(id);
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
			console::error(fmt::format("Could not apply action: entity '{}' not found", static_cast<std::underlying_type_t<entity_id>>(get_id())));
			return;
		}

		do_apply(*b);
	}

	void single_brush::undo(map& current_map)
	{
		brush* b = current_map.find_brush(get_id());
		if (b == nullptr)
		{
			console::error(fmt::format("Could not undo action: entity '{}' not found", static_cast<std::underlying_type_t<entity_id>>(get_id())));
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

	split_brush_edge::split_brush_edge(brush const& b, egfx::half_edge::id edge, math::point3f point)
		: brush_definition_base(b)
		, edge(edge)
		, point(point)
	{

	}

	void split_brush_edge::do_apply(brush& b)
	{
		auto new_mesh = std::make_shared<egfx::mesh_definition>(*b.mesh_def);
		new_mesh->get_half_edge(edge).split_at(point);
		b.reload_node(std::move(new_mesh));
	}

	split_brush_face::split_brush_face(brush const& b, egfx::face::id face, math::plane plane)
		: brush_definition_base(b)
		, face(face)
		, plane(plane)
	{

	}

	void split_brush_face::do_apply(brush& b)
	{
		auto new_mesh = std::make_shared<egfx::mesh_definition>(*b.mesh_def);
		auto result = new_mesh->get_face(face).split(plane);
		if (result)
		{
			egfx::face::ref const new_face = *result;
			console::log(fmt::format("Split brush {} face {} into new face {}", get_id(), face, new_face.get_id()));
		}
		else
		{
			egfx::face::split_fail const fail = result.error();
			switch (fail)
			{
			case egfx::face::split_fail::inside: 
				console::error(fmt::format("Could not split brush {} face {}: face was entirely inside the plane", get_id(), face));
				break;
			case egfx::face::split_fail::outside:
				console::error(fmt::format("Could not split brush {} face {}: face was entirely outside the plane", get_id(), face));
				break;
			case egfx::face::split_fail::aligned:
				console::error(fmt::format("Could not split brush {} face {}: face was aligned to the plane", get_id(), face));
				break;
			case egfx::face::split_fail::opposite_aligned:
				console::error(fmt::format("Could not split brush {} face {}: face was opposite-aligned to the plane", get_id(), face));
				break;
			}
		}
		b.reload_node(std::move(new_mesh));
	}

	set_brush_position::set_brush_position(brush const& b, math::point3f point)
		: single_brush(b)
		, previous_state(b.node.get_position())
		, new_pos(point)
	{

	}

	void set_brush_position::do_apply(brush& b)
	{
		b.node.set_position(new_pos);
	}

	void set_brush_position::do_undo(brush& b)
	{
		b.node.set_position(previous_state);
	}

	set_brush_rotation::set_brush_rotation(brush const& b, math::quaternion rot)
		: single_brush(b)
		, previous_state(b.node.get_rotation())
		, new_rot(rot)
	{

	}

	void set_brush_rotation::do_apply(brush& b)
	{
		b.node.set_rotation(new_rot);
	}

	void set_brush_rotation::do_undo(brush& b)
	{
		b.node.set_rotation(previous_state);
	}

	set_brush_scale::set_brush_scale(brush const& b, math::scales s)
		: single_brush(b)
		, previous_state(b.node.get_scale())
		, new_s(s)
	{

	}

	void set_brush_scale::do_apply(brush& b)
	{
		b.node.set_scale(new_s);
	}

	void set_brush_scale::do_undo(brush& b)
	{
		b.node.set_scale(previous_state);
	}
}
