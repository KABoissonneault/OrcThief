#include "action/brush.h"

#include "console.h"
#include "serialize/serialize_map.h"
#include "egfx/mesh_definition.h"

#include <cassert>
#include <format>

namespace ot::dedit::action
{
	spawn_brush::spawn_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def)
		: mesh_def(std::move(mesh_def))
		, id()
	{

	}

	spawn_brush::spawn_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id parent_id)
		: mesh_def(mesh_def)
		, id()
		, parent_id(parent_id)
	{
		
	}

	void spawn_brush::apply(map& current_map)
	{
		do_spawn(current_map, false);
	}

	void spawn_brush::redo(map& current_map)
	{
		do_spawn(current_map, true);
	}
	
	void spawn_brush::undo(map& current_map)
	{
		if (!id)
			throw std::logic_error("spawn_brush::undo called before apply");
		current_map.delete_brush(*id);
	}

	void spawn_brush::do_spawn(map& current_map, bool is_redo)
	{
		if (!is_redo)
			id = current_map.allocate_entity_id();
		else
			assert(id);

		if (parent_id)
		{
			map_entity* parent = current_map.find_entity(*parent_id);
			if (parent == nullptr)
			{
				console::error(std::format("Failed to spawn brush: parent id '{}' not found", as_int(*parent_id)));
			}
			else
			{
				current_map.make_brush(mesh_def, *id, *parent);
				if (!is_redo)
					console::log(std::format("Created brush {} under {}", as_int(*id), parent->get_name()));
			}
		}
		else
		{
			current_map.make_brush(mesh_def, *id);
			if (!is_redo)
				console::log(std::format("Created brush {}", as_int(*id)));
		}
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
			console::error(std::format("Could not apply action: entity '{}' not found", as_int(get_id())));
			return;
		}

		do_apply(*b, false);
	}

	void single_brush::redo(map& current_map)
	{
		brush* b = current_map.find_brush(get_id());
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

	brush_definition_base::brush_definition_base(brush const& b)
		: single_brush(b)
		, previous_state(b.get_shared_mesh_def())
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

	void split_brush_edge::do_apply(brush& b, bool is_redo)
	{
		(void)is_redo;

		auto new_mesh = std::make_shared<egfx::mesh_definition>(b.get_mesh_def());
		new_mesh->get_half_edge(edge).split_at(point);
		b.reload_node(std::move(new_mesh));
	}

	split_brush_face::split_brush_face(brush const& b, egfx::face::id face, math::plane plane)
		: brush_definition_base(b)
		, face(face)
		, plane(plane)
	{

	}

	void split_brush_face::do_apply(brush& b, bool is_redo)
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

	set_brush_position::set_brush_position(brush const& b, math::point3f point)
		: single_brush(b)
		, previous_state(b.get_node().get_position())
		, new_pos(point)
	{

	}

	void set_brush_position::do_apply(brush& b, bool is_redo)
	{
		(void)is_redo;
		b.get_node().set_position(new_pos);
	}

	void set_brush_position::do_undo(brush& b)
	{
		b.get_node().set_position(previous_state);
	}

	set_brush_rotation::set_brush_rotation(brush const& b, math::quaternion rot)
		: single_brush(b)
		, previous_state(b.get_node().get_rotation())
		, new_rot(rot)
	{

	}

	void set_brush_rotation::do_apply(brush& b, bool is_redo)
	{
		(void)is_redo;
		b.get_node().set_rotation(new_rot);
	}

	void set_brush_rotation::do_undo(brush& b)
	{
		b.get_node().set_rotation(previous_state);
	}

	set_brush_scale::set_brush_scale(brush const& b, math::scales s)
		: single_brush(b)
		, previous_state(b.get_node().get_scale())
		, new_s(s)
	{

	}

	void set_brush_scale::do_apply(brush& b, bool is_redo)
	{
		(void)is_redo;
		b.get_node().set_scale(new_s);
	}

	void set_brush_scale::do_undo(brush& b)
	{
		b.get_node().set_scale(previous_state);
	}

	delete_brush::delete_brush(brush const& b)
		: id(b.get_id())
		, previous_parent(b.get_parent()->get_id())
		, serialized_state(nullptr, &std::fclose)
	{

	}

	void delete_brush::apply(map& current_map)
	{
		do_delete(current_map, false);
	}

	void delete_brush::redo(map& current_map)
	{
		do_delete(current_map, true);
	}

	void delete_brush::undo(map& current_map)
	{
		map_entity* const parent_entity = current_map.find_entity(previous_parent);
		if (parent_entity == nullptr)
		{
			console::error(std::format("Could not undo 'delete_brush' action: parent entity '{}' not found", as_int(id)));
			return;
		}

		if (!serialize::fread(current_map, *parent_entity, serialized_state.get()))
		{
			console::error(std::format("Could not undo 'delete_brush' action: failed to deserialize brush '{}'", as_int(id)));
			return;
		}
	}

	void delete_brush::do_delete(map& current_map, bool is_redo)
	{
		std::FILE* f;
		errno_t const err = tmpfile_s(&f);
		if (err != 0)
		{
			console::error("Could not apply 'delete_brush' action: temporary buffer for preserving state could not be acquired");
			return;
		}

		serialized_state.reset(f);

		brush* const b = current_map.find_brush(id);
		if (b == nullptr)
		{
			console::error(std::format("Could not apply 'delete_brush' action: entity '{}' not found", as_int(id)));
			return;
		}

		if (!serialize::fwrite(*b, serialized_state.get()))
		{
			console::error(std::format("Could not apply 'delete_brush' action: failed to serialize entity '{}'", as_int(id)));
			return;
		}

		fseek(serialized_state.get(), 0, SEEK_SET);

		current_map.delete_brush(id);
		if(!is_redo)
			console::log(std::format("Deleted brush {}", as_int(id)));
	}

	set_brush_material::set_brush_material(brush const& b, egfx::material_handle_t const& mat)
		: single_brush(b)
		, new_material(mat)
		, previous_material(b.get_mesh_node().get_material())
	{

	}

	void set_brush_material::do_apply(brush& b, bool is_redo)
	{
		(void)is_redo;
		b.get_mesh_node().set_material(new_material);
	}

	void set_brush_material::do_undo(brush& b)
	{
		b.get_mesh_node().set_material(previous_material);
	}
}
