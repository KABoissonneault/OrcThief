#include "action/brush.h"

#include "console.h"
#include "serialize/serialize_map.h"
#include "egfx/mesh_definition.h"

#include <cassert>
#include <format>

namespace ot::dedit::action
{
	single_brush::single_brush(brush_entity const& b)
		: id(b.get_id())
	{

	}

	void single_brush::apply(map& current_map)
	{
		brush_entity* b = current_map.find_brush(get_id());
		if (b == nullptr)
		{
			console::error(std::format("Could not apply action: entity '{}' not found", as_int(get_id())));
			return;
		}

		do_apply(*b, false);
	}

	void single_brush::redo(map& current_map)
	{
		brush_entity* b = current_map.find_brush(get_id());
		if (b == nullptr)
		{
			console::error(std::format("Could not redo action: entity '{}' not found", as_int(get_id())));
			return;
		}

		do_apply(*b, true);
	}

	void single_brush::undo(map& current_map)
	{
		brush* b = current_map.find_brush(get_id());
		assert(b != nullptr);
		do_undo(*b);
	}

	brush_definition_base::brush_definition_base(brush_entity const& b)
		: single_brush(b)
		, previous_state(b.get_shared_mesh_def())
	{

	}

	void brush_definition_base::do_undo(brush_entity& b)
	{
		b.reload_node(std::move(previous_state));
	}

	split_brush_edge::split_brush_edge(brush_entity const& b, egfx::half_edge::id edge, math::point3f point)
		: brush_definition_base(b)
		, edge(edge)
		, point(point)
	{

	}

	void split_brush_edge::do_apply(brush_entity& b, bool is_redo)
	{
		(void)is_redo;

		auto new_mesh = std::make_shared<egfx::mesh_definition>(b.get_mesh_def());
		new_mesh->get_half_edge(edge).split_at(point);
		b.reload_node(std::move(new_mesh));
	}

	split_brush_face::split_brush_face(brush_entity const& b, egfx::face::id face, math::plane plane)
		: brush_definition_base(b)
		, face(face)
		, plane(plane)
	{

	}

	void split_brush_face::do_apply(brush_entity& b, bool is_redo)
	{
		auto new_mesh = std::make_shared<egfx::mesh_definition>(b.get_mesh_def());
		auto result = new_mesh->get_face(face).split(plane);
		if (result)
		{
			egfx::face::ref const new_face = *result;
			if(!is_redo)
				console::log(std::format( "Split brush {} face {} into new face {}", as_int(get_id()), static_cast<size_t>(face), static_cast<size_t>(new_face.get_id())));
			b.reload_node(std::move(new_mesh));
		}
		else
		{
			egfx::face::split_fail const fail = result.error();
			switch (fail)
			{
			case egfx::face::split_fail::inside: 
				console::error(std::format("Could not split brush {} face {}: face was entirely inside the plane", as_int(get_id()), static_cast<size_t>(face)));
				break;
			case egfx::face::split_fail::outside:
				console::error(std::format("Could not split brush {} face {}: face was entirely outside the plane", as_int(get_id()), static_cast<size_t>(face)));
				break;
			case egfx::face::split_fail::aligned:
				console::error(std::format("Could not split brush {} face {}: face was aligned to the plane", as_int(get_id()), static_cast<size_t>(face)));
				break;
			case egfx::face::split_fail::opposite_aligned:
				console::error(std::format("Could not split brush {} face {}: face was opposite-aligned to the plane", as_int(get_id()), static_cast<size_t>(face)));
				break;
			}
		}		
	}
			
	set_brush_material::set_brush_material(brush_entity const& b, egfx::material_handle_t const& mat)
		: single_brush(b)
		, new_material(mat)
		, previous_material(b.get_item().get_material())
	{

	}

	void set_brush_material::do_apply(brush_entity& b, bool is_redo)
	{
		(void)is_redo;
		b.get_item().set_material(new_material);
	}

	void set_brush_material::do_undo(brush_entity& b)
	{
		b.get_item().set_material(previous_material);
	}
}
