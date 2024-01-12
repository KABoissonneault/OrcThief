#pragma once

#include "base.h"

#include "map.fwd.h"

#include "core/uptr.h"

#include "egfx/mesh_definition.fwd.h"
#include "egfx/object/light.fwd.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <cstdio>
#include <optional>
#include <memory>

namespace ot::dedit::action
{	
	class delete_entity : public base
	{
		entity_id id;
		entity_id previous_parent;
		uptr<std::FILE, int(*)(std::FILE*)> serialized_state;

		void do_delete(map& current_map, bool is_redo);

	protected:
		virtual void apply(map& current_map) override;
		virtual void redo(map& current_map) override;
		virtual void undo(map& current_map) override;

	public:
		delete_entity(map_entity const& e);
	};

	template<typename EntityType, typename... Args>
	class spawn_entity : public base
	{
		std::optional<entity_id> id;
		entity_id parent_id;
		std::tuple<Args...> extra_args;

		void do_spawn(map& current_map, bool is_redo);

	public:
		template<typename... ConstructorArgs>
		spawn_entity(entity_id parent_id, ConstructorArgs&&... args)
			: parent_id(parent_id)
			, extra_args(std::make_tuple(ot::forward<ConstructorArgs>(args)...))
		{

		}

		virtual void apply(map& current_map) override;
		virtual void redo(map& current_map) override;
		virtual void undo(map& current_map) override;
	};

	extern template class spawn_entity<brush_entity, std::shared_ptr<egfx::mesh_definition const>>;

	class spawn_brush : public spawn_entity<brush_entity, std::shared_ptr<egfx::mesh_definition const>>
	{
	public:
		spawn_brush(entity_id parent_id, std::shared_ptr<egfx::mesh_definition const> mesh_def);
	};

	extern template class spawn_entity<light_entity, egfx::light_type>;

	class spawn_light : public spawn_entity<light_entity, egfx::light_type>
	{
	public:
		spawn_light(entity_id parent_id, egfx::light_type light_type);
	};

	class single_entity : public base
	{
		entity_id id;

	protected:
		single_entity(map_entity const& e);

		entity_id get_id() const noexcept { return id; }

		virtual void do_apply(map_entity& e, bool is_redo) = 0;
		virtual void do_undo(map_entity& e) = 0;

	public:
		virtual void apply(map& current_map) override final;
		virtual void redo(map& current_map) override final;
		virtual void undo(map& current_map) override final;
	};


	class set_entity_position : public single_entity
	{
		math::point3f previous_state;
		math::point3f new_pos;

	protected:
		virtual void do_apply(map_entity& e, bool is_redo) override;
		virtual void do_undo(map_entity& e) override;

	public:
		set_entity_position(map_entity const& b, math::point3f point);
	};

	class set_entity_rotation : public single_entity
	{
		math::quaternion previous_state;
		math::quaternion new_rot;

	protected:
		virtual void do_apply(map_entity& e, bool is_redo) override;
		virtual void do_undo(map_entity& e) override;

	public:
		set_entity_rotation(map_entity const& e, math::quaternion rot);
	};

	class set_entity_scale : public single_entity
	{
		math::scales previous_state;
		math::scales new_s;

	protected:
		virtual void do_apply(map_entity& e, bool is_redo) override;
		virtual void do_undo(map_entity& e) override;

	public:
		set_entity_scale(map_entity const& e, math::scales s);
	};

	class single_object : public base
	{
		entity_id e_id;
		egfx::object_id object_id;

	protected:
		single_object(egfx::object_cref object);

		entity_id get_entity_id() const noexcept { return e_id; }
		egfx::object_id get_object_id() const noexcept { return object_id; }

		virtual void do_apply(egfx::object_ref o, bool is_redo) = 0;
		virtual void do_undo(egfx::object_ref o) = 0;

	public:
		virtual void apply(map& current_map) override final;
		virtual void redo(map& current_map) override final;
		virtual void undo(map& current_map) override final;
	};

	class set_object_casts_shadows : public single_object
	{
		bool new_value;
		bool old_value;

	protected:
		virtual void do_apply(egfx::object_ref o, bool is_redo) override;
		virtual void do_undo(egfx::object_ref o) override;

	public:
		set_object_casts_shadows(egfx::object_cref object, bool new_value);
	};
}
