#include "map_entity.h"

#include "map.h"
#include "console.h"

#include "serialize/serialize_map.h"

#include <format>

namespace ot::dedit::action
{
	delete_entity::delete_entity(map_entity const& e)
		: id(e.get_id())
		, previous_parent(e.get_parent()->get_id())
		, serialized_state(nullptr, &std::fclose)
	{

	}

	void delete_entity::apply(map& current_map)
	{
		do_delete(current_map, false);
	}

	void delete_entity::redo(map& current_map)
	{
		do_delete(current_map, true);
	}

	void delete_entity::undo(map& current_map)
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

	void delete_entity::do_delete(map& current_map, bool is_redo)
	{
		std::FILE* f;
		errno_t const err = tmpfile_s(&f);
		if (err != 0)
		{
			console::error("Could not apply 'delete_entity' action: temporary buffer for preserving state could not be acquired");
			return;
		}

		serialized_state.reset(f);

		map_entity* const b = current_map.find_entity(id);
		if (b == nullptr)
		{
			console::error(std::format("Could not apply 'delete_entity' action: entity '{}' not found", as_int(id)));
			return;
		}

		std::string_view const type_name = as_string(b->get_type());

		if (!serialize::fwrite(*b, serialized_state.get()))
		{
			console::error(std::format("Could not apply 'delete_entity' action: failed to serialize entity '{}'", as_int(id)));
			return;
		}

		fseek(serialized_state.get(), 0, SEEK_SET);

		current_map.delete_entity(id);
		if (!is_redo)
			console::log(std::format("Deleted {} {}", type_name, as_int(id)));
	}
		
	template<typename EntityType, typename... Args>
	void spawn_entity<EntityType, Args...>::apply(map& current_map)
	{
		do_spawn(current_map, false);
	}

	template<typename EntityType, typename... Args>
	void spawn_entity<EntityType, Args...>::redo(map& current_map)
	{
		do_spawn(current_map, true);
	}

	template<typename EntityType, typename... Args>
	void spawn_entity<EntityType, Args...>::undo(map& current_map)
	{
		if (!id)
			throw std::logic_error("spawn_entity::undo called before apply");
		current_map.delete_entity(*id);
	}

	template<typename EntityType, typename... Args>
	void spawn_entity<EntityType, Args...>::do_spawn(map& current_map, bool is_redo)
	{
		constexpr std::string_view type_name = as_string(EntityType::type);

		if (!is_redo)
			id = current_map.allocate_entity_id();
		else
			assert(id);

		map_entity* const parent = current_map.find_entity(parent_id);
		if (parent == nullptr)
		{
			console::error(std::format("Failed to spawn {}: parent id '{}' not found", type_name, as_int(parent_id)));
			return;
		}

		[&current_map, parent, this] <size_t... Is>(std::index_sequence<Is...>)
		{
			current_map.make_entity<EntityType>(*id, *parent, std::get<Is>(extra_args)...);
		}(std::index_sequence_for<Args...>{});
		
		if (!is_redo)
			console::log(std::format("Created {} {} under {}", type_name, as_int(*id), parent->get_name()));
	}

	spawn_brush::spawn_brush(entity_id parent_id, std::shared_ptr<egfx::mesh_definition const> mesh_def)
		: spawn_entity(parent_id, as_movable(mesh_def))
	{

	}

	template class spawn_entity<brush_entity, std::shared_ptr<egfx::mesh_definition const>>;

	spawn_light::spawn_light(entity_id parent_id, egfx::light_type light_type)
		: spawn_entity(parent_id, light_type)
	{

	}

	template class spawn_entity<light_entity, egfx::light_type>;

	single_entity::single_entity(map_entity const& e)
		: id(e.get_id())
	{

	}

	void single_entity::apply(map& current_map)
	{
		map_entity* const e = current_map.find_entity(get_id());
		if (e == nullptr)
		{
			console::error(std::format("Could not apply action: entity '{}' not found", as_int(get_id())));
			return;
		}

		do_apply(*e, false);
	}

	void single_entity::redo(map& current_map)
	{
		map_entity* const e = current_map.find_entity(get_id());
		if (e == nullptr)
		{
			console::error(std::format("Could not redo action: entity '{}' not found", as_int(get_id())));
			return;
		}

		do_apply(*e, true);
	}

	void single_entity::undo(map& current_map)
	{
		map_entity* const e = current_map.find_entity(get_id());
		assert(e != nullptr);
		do_undo(*e);
	}

	set_entity_position::set_entity_position(map_entity const& e, math::point3f point)
		: single_entity(e)
		, previous_state(e.get_node().get_position())
		, new_pos(point)
	{

	}

	void set_entity_position::do_apply(map_entity& e, bool is_redo)
	{
		(void)is_redo;
		e.get_node().set_position(new_pos);
	}

	void set_entity_position::do_undo(map_entity& b)
	{
		b.get_node().set_position(previous_state);
	}

	set_entity_rotation::set_entity_rotation(map_entity const& b, math::quaternion rot)
		: single_entity(b)
		, previous_state(b.get_node().get_rotation())
		, new_rot(rot)
	{

	}

	void set_entity_rotation::do_apply(map_entity& e, bool is_redo)
	{
		(void)is_redo;
		e.get_node().set_rotation(new_rot);
	}

	void set_entity_rotation::do_undo(map_entity& e)
	{
		e.get_node().set_rotation(previous_state);
	}

	set_entity_scale::set_entity_scale(map_entity const& e, math::scales s)
		: single_entity(e)
		, previous_state(e.get_node().get_scale())
		, new_s(s)
	{

	}

	void set_entity_scale::do_apply(map_entity& e, bool is_redo)
	{
		(void)is_redo;
		e.get_node().set_scale(new_s);
	}

	void set_entity_scale::do_undo(map_entity& e)
	{
		e.get_node().set_scale(previous_state);
	}

	namespace
	{
		entity_id get_owning_entity_id(egfx::object_cref object)
		{
			egfx::node_cref scene_node = object.get_owner();
			map_entity const* entity = static_cast<map_entity const*>(scene_node.get_user_ptr());
			assert(entity);
			return entity->get_id();
		}
	}

	single_object::single_object(egfx::object_cref object)
		: e_id(get_owning_entity_id(object))
		, object_id(object.get_object_id())
	{

	}

	void single_object::apply(map& current_map)
	{
		map_entity* const e = current_map.find_entity(e_id);
		if (e == nullptr)
		{
			console::error(std::format("Could not apply action: entity '{}' not found", as_int(e_id)));
			return;
		}

		egfx::node_ref const node = e->get_node();
		auto const objects = node.get_objects();
		auto const found_it = std::ranges::find(objects, object_id, &egfx::object_ref::get_object_id);
		if (found_it == objects.end())
		{
			console::error(std::format("Could not apply action: object '{}' under entity '{}' not found", as_int(object_id), as_int(e_id)));
			return;
		}

		do_apply(*found_it, false);
	}

	void single_object::redo(map& current_map)
	{
		map_entity* const e = current_map.find_entity(e_id);
		if (e == nullptr)
		{
			console::error(std::format("Could not redo action: entity '{}' not found", as_int(e_id)));
			return;
		}

		egfx::node_ref const node = e->get_node();
		auto const objects = node.get_objects();
		auto const found_it = std::ranges::find(objects, object_id, &egfx::object_ref::get_object_id);
		if (found_it == objects.end())
		{
			console::error(std::format("Could not redo action: object '{}' under entity '{}' not found", as_int(object_id), as_int(e_id)));
			return;
		}

		do_apply(*found_it, true);
	}

	void single_object::undo(map& current_map)
	{
		map_entity* const e = current_map.find_entity(e_id);
		if (e == nullptr)
		{
			console::error(std::format("Could not undo action: entity '{}' not found", as_int(e_id)));
			return;
		}

		egfx::node_ref const node = e->get_node();
		auto const objects = node.get_objects();
		auto const found_it = std::ranges::find(objects, object_id, &egfx::object_ref::get_object_id);
		if (found_it == objects.end())
		{
			console::error(std::format("Could not undo action: object '{}' under entity '{}' not found", as_int(object_id), as_int(e_id)));
			return;
		}

		do_undo(*found_it);
	}

	set_object_casts_shadows::set_object_casts_shadows(egfx::object_cref object, bool new_value)
		: single_object(object)
		, new_value(new_value)
		, old_value(object.is_casting_shadows())
	{

	}

	void set_object_casts_shadows::do_apply(egfx::object_ref o, bool is_redo)
	{
		(void)is_redo;
		o.set_casting_shadows(new_value);
	}

	void set_object_casts_shadows::do_undo(egfx::object_ref o)
	{
		o.set_casting_shadows(old_value);
	}

}