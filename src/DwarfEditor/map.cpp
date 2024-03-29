#include "map.h"

#include "serialize/serialize_mesh_definition.h"
#include "serialize/serialize_math.h"

#include <format>
#include <cassert>

namespace ot::dedit
{
	namespace
	{
		std::string make_brush_name(entity_id id)
		{
			return std::format("Brush {}", as_int(id));
		}
	}

	map_entity_iterator::map_entity_iterator(egfx::node_iterator it, egfx::node_iterator end) noexcept
		: node_iterator(it)
		, node_end(end)
	{
		while (node_iterator != node_end && node_iterator->get_user_ptr() == nullptr)
			++node_iterator;
	}

	map_entity_iterator& map_entity_iterator::operator++()
	{
		do
		{
			++node_iterator;
		} while (node_iterator != node_end && node_iterator->get_user_ptr() == nullptr);

		return *this;
	}

	map_entity_iterator map_entity_iterator::operator++(int)
	{
		map_entity_iterator prev = *this;
		++(*this);
		return prev;
	}

	auto map_entity_iterator::operator*() const -> reference
	{
		egfx::node_ref node = *node_iterator;
		return *static_cast<map_entity*>(node.get_user_ptr());
	}

	auto map_entity_iterator::operator->() const -> pointer
	{
		egfx::node_ref node = *node_iterator;
		return static_cast<map_entity*>(node.get_user_ptr());
	}

	bool map_entity_iterator::operator==(map_entity_iterator const& rhs) const noexcept
	{
		return node_iterator == rhs.node_iterator && node_end == rhs.node_end;
	}

	bool map_entity_iterator::operator==(map_entity_end_sentinel const&) const noexcept
	{
		return node_iterator == node_end;
	}

	map_entity_range::map_entity_range(map_entity_iterator beg) noexcept
		: it(beg)
	{

	}

	map_entity_range::map_entity_range(egfx::node_range node_range) noexcept
		: it(node_range.begin(), node_range.end())
	{

	}

	map_entity_const_iterator::map_entity_const_iterator(egfx::node_const_iterator it, egfx::node_const_iterator end) noexcept
		: node_iterator(it)
		, node_end(end)
	{
		while (node_iterator != node_end && node_iterator->get_user_ptr() == nullptr)
			++node_iterator;
	}

	map_entity_const_iterator::map_entity_const_iterator(map_entity_iterator it) noexcept
		: node_iterator(it.node_iterator)
		, node_end(it.node_end)
	{

	}

	map_entity_const_iterator& map_entity_const_iterator::operator++()
	{
		do
		{
			++node_iterator;
		} while (node_iterator != node_end && node_iterator->get_user_ptr() == nullptr);

		return *this;
	}

	map_entity_const_iterator map_entity_const_iterator::operator++(int)
	{
		map_entity_const_iterator prev = *this;
		++(*this);
		return prev;
	}

	auto map_entity_const_iterator::operator*() const -> reference
	{
		egfx::node_cref node = *node_iterator;
		return *static_cast<map_entity const*>(node.get_user_ptr());
	}

	auto map_entity_const_iterator::operator->() const -> pointer
	{
		egfx::node_cref node = *node_iterator;
		return static_cast<map_entity const*>(node.get_user_ptr());
	}

	bool map_entity_const_iterator::operator==(map_entity_const_iterator const& rhs) const noexcept
	{
		return node_iterator == rhs.node_iterator && node_end == rhs.node_end;
	}

	bool map_entity_const_iterator::operator==(map_entity_end_sentinel const&) const noexcept
	{
		return node_iterator == node_end;
	}

	map_entity_const_range::map_entity_const_range(map_entity_const_iterator beg) noexcept
		: it(beg)
	{

	}

	map_entity_const_range::map_entity_const_range(map_entity_range range) noexcept
		: it(range.it)
	{

	}

	map_entity_const_range::map_entity_const_range(egfx::node_const_range range) noexcept
		: it(range.begin(), range.end())
	{

	}

	map_entity::map_entity(entity_id id)
		: id(id)
	{

	}

	map_entity const* map_entity::get_parent() const noexcept
	{
		egfx::node_cref const node = get_node();
		auto const parent = node.get_parent();
		if (!parent)
			return nullptr;
		return static_cast<map_entity const*>(parent->get_user_ptr());
	}

	map_entity* map_entity::get_parent() noexcept
	{
		egfx::node_ref const node = get_node();
		auto const parent = node.get_parent();
		if (!parent)
			return nullptr;
		return static_cast<map_entity*>(parent->get_user_ptr());
	}

	map_entity_const_range map_entity::get_children() const noexcept
	{
		egfx::node_cref const node = get_node();
		egfx::node_const_range const child_objects = node.get_children();
		egfx::node_const_iterator it = child_objects.begin();
		egfx::node_const_iterator const end = child_objects.end();
		while (it != end && it->get_user_ptr() == nullptr)
			++it;
		return map_entity_const_range(egfx::node_const_range(it, end));
	}

	map_entity_range map_entity::get_children() noexcept
	{
		egfx::node_ref const node = get_node();
		egfx::node_range const child_objects = node.get_children();
		egfx::node_iterator it = child_objects.begin();
		egfx::node_iterator const end = child_objects.end();
		while (it != end && it->get_user_ptr() == nullptr)
			++it;
		return map_entity_range(egfx::node_range(it, end));
	}

	map_entity* map_entity::find_recursive(entity_id search_id)
	{
		map_entity* result = nullptr;
		for_each_recursive([search_id, &result](map_entity& e)
		{
			if (e.get_id() == search_id)
			{
				result = &e;
				return true;
			}

			return false;
		});
		return result;
	}

	map_entity const* map_entity::find_recursive(entity_id search_id) const
	{
		map_entity const* result = nullptr;
		for_each_recursive([search_id, &result](map_entity const& e)
		{
			if (e.get_id() == search_id)
			{
				result = &e;
				return true;
			}

			return false;
		});
		return result;
	}

	math::transform_matrix map_entity::get_local_transform() const noexcept
	{
		egfx::node_cref const node = get_node();
		return math::transform_matrix::from_components(vector_from_origin(node.get_position()), node.get_rotation(), node.get_scale());
	}

	math::transform_matrix map_entity::get_world_transform() const noexcept
	{
		if (map_entity const* parent = get_parent())
			return parent->get_world_transform() * get_local_transform();
		return get_local_transform();
	}

	void map_entity::set_world_transform(math::transform_matrix const& m) noexcept
	{
		egfx::node_ref const node = get_node();

		math::transform_matrix new_local;
		if (map_entity const* parent = get_parent())
		{
			math::transform_matrix const parent_world = parent->get_world_transform();
			math::transform_matrix const parent_inverse = invert(parent_world);
			new_local = m * parent_inverse;
		}
		else
		{
			new_local = m;
		}

		math::point3f const new_position = math::point3f() + new_local.get_displacement();
		math::rotation_matrix const new_rotation = new_local.get_rotation();
		math::scales const new_scale = new_local.get_scale();
		node.set_position(new_position);
		node.set_rotation(new_rotation.to_quaternion());
		node.set_scale(new_scale);
	}

	root_entity::root_entity(entity_id id, egfx::node_ref node)
		: map_entity(id)
		, node(node)
	{
		node.set_user_ptr(this);
	}

	node_entity::node_entity(entity_id id)
		: map_entity(id)
	{

	}

	node_entity::node_entity(entity_id id, map_entity& parent, std::string_view name)
		: map_entity(id)
		, node(egfx::create_child_node(parent.get_node()))
	{
		node.set_name(name);
		node.set_user_ptr(this);
	}

	bool node_entity::fwrite(std::FILE* f) const
	{
		std::string_view const name = node.get_name();
		size_t const name_size = name.size();
		if(!::fwrite(&name_size, sizeof(name_size), 1, f))
			return false;

		if (!::fwrite(name.data(), 1, name.size(), f))
			return false;

		if (!serialize::fwrite(node.get_position(), f)
			|| !serialize::fwrite(node.get_rotation(), f)
			|| !serialize::fwrite(node.get_scale(), f))
			return false;

		return true;
	}

	bool node_entity::fread(map_entity& parent, std::FILE* f)
	{
		size_t name_size;
		if (!::fread(&name_size, sizeof(name_size), 1, f))
			return false;

		std::string name;
		name.resize(name_size);
		if (!::fread(name.data(), 1, name_size, f))
			return false;

		math::point3f position;
		math::quaternion rotation;
		math::scales scales;
		if (!serialize::fread(position, f) || !serialize::fread(rotation, f) || !serialize::fread(scales, f))
			return false;
		
		node = egfx::create_child_node(parent.get_node());

		node.set_name(name);
		node.set_position(position);
		node.set_rotation(rotation);
		node.set_scale(scales);

		node.set_user_ptr(this);

		return true;
	}

	brush_entity::brush_entity(entity_id id)
		: node_entity(id)
	{

	}

	brush_entity::brush_entity(entity_id id, map_entity& parent, std::shared_ptr<egfx::mesh_definition const> mesh_def)
		: node_entity(id, parent, make_brush_name(id))
		, mesh_def(mesh_def)
		, mesh(egfx::create_mesh(make_brush_name(id), *mesh_def))
	{
		egfx::node_ref const node_ref = get_node();
		egfx::add_item(node_ref, mesh);
	}

	bool brush::fwrite(std::FILE* f) const
	{
		if (!node_entity::fwrite(f))
			return false;

		if (!serialize::fwrite(*mesh_def, f))
			return false;

		// TODO: material

		return true;
	}

	bool brush::fread(map_entity& parent, std::FILE* f)
	{
		if (!node_entity::fread(parent, f))
			return false;

		auto read_mesh_def = std::make_shared<egfx::mesh_definition>();
		if (!serialize::fread(*read_mesh_def, f))
			return false;
		
		mesh_def = std::move(read_mesh_def);
		mesh = egfx::create_mesh(make_brush_name(get_id()), *mesh_def);

		egfx::item_ref const brush = egfx::add_item(get_node(), mesh);

		// TODO: material
		
		return true;
	}

	void brush::reload_node(std::shared_ptr<egfx::mesh_definition const> new_def)
	{
		mesh_def = std::move(new_def);
		mesh.reload_mesh(*mesh_def);
	}

	light_entity::light_entity(entity_id id)
		: node_entity(id)
	{

	}

	light_entity::light_entity(entity_id id, map_entity& parent, egfx::light_type light_type)
		: node_entity(id, parent, std::format("Light {}", as_int(id)))
	{
		egfx::add_light(get_node(), light_type);
	}

	bool light_entity::fwrite(std::FILE* f) const
	{
		if (!node_entity::fwrite(f))
			return false;

		egfx::light_cref const light = get_light();

		egfx::light_type const light_type = light.get_light_type();
		if (!::fwrite(&light_type, sizeof(light_type), 1, f))
			return false;

		float const power_scale = light.get_power_scale();
		if (!::fwrite(&power_scale, sizeof(power_scale), 1, f))
			return false;

		egfx::color const diffuse = light.get_diffuse();
		if (!::fwrite(&diffuse, sizeof(float), 3 /*don't write alpha*/, f))
			return false;

		return true;
	}

	bool light_entity::fread(map_entity& parent, std::FILE* f)
	{
		if (!node_entity::fread(parent, f))
			return false;

		egfx::light_type light_type;
		if (!::fread(&light_type, sizeof(light_type), 1, f))
			return false;

		float power_scale;
		if (!::fread(&power_scale, sizeof(power_scale), 1, f))
			return false;

		egfx::color diffuse;
		if (!::fread(&diffuse, sizeof(float), 3, f))
			return false;
		diffuse.a = 1.0f;

		egfx::light_ref light = egfx::add_light(get_node(), light_type);
		light.set_power_scale(power_scale);
		light.set_diffuse(diffuse);

		return true;
	}

	map::map(egfx::node_ref root_node)
		: root(allocate_entity_id(), root_node)
	{

	}

	entity_id map::allocate_entity_id()
	{
		return static_cast<entity_id>(next_entity_id++);
	}

	void map::on_new_entity(entity_id id)
	{
		assert(find_entity(id) == nullptr);
		if (next_entity_id <= as_int(id))
			next_entity_id = as_int(id) + 1;
	}

	void map::delete_entity(entity_id id)
	{
		map_entity const* deleted_parent = find_entity(id);
		if (deleted_parent == nullptr)
			return;

		auto const [removed_it, removed_sent] = std::ranges::remove_if(entities, [deleted_parent](uptr<map_entity> const& e) -> bool
		{
			return deleted_parent->find_recursive(e->get_id()) != nullptr;
		});

		entities.erase(removed_it, removed_sent);
	}

	void map::clear()
	{
		entities.clear();
		next_entity_id = 1; // Root always has id 0
	}

	brush_entity const* map::find_brush(entity_id id) const noexcept
	{
		map_entity const* found_entity = find_entity(id);
		if (found_entity == nullptr)
			return nullptr;

		assert(found_entity->get_type() == entity_type::brush);

		return static_cast<brush_entity const*>(found_entity);
	}

	brush_entity* map::find_brush(entity_id id) noexcept
	{
		return const_cast<brush*>(static_cast<map const*>(this)->find_brush(id));
	}

	map_entity_const_range map::get_root_entities() const
	{
		return root.get_children();
	}

	map_entity* map::find_entity(entity_id id)
	{
		if (id == entity_id::root)
			return &root;

		auto const it_found = std::find_if(entities.begin(), entities.end(), [id](uptr<map_entity> const& b) { return b->get_id() == id; });
		if (it_found == entities.end())
			return nullptr;

		return (*it_found).get();
	}

	map_entity const* map::find_entity(entity_id id) const
	{
		if (id == entity_id::root)
			return &root;

		auto const it_found = std::find_if(entities.begin(), entities.end(), [id](uptr<map_entity> const& b) { return b->get_id() == id; });
		if (it_found == entities.end())
			return nullptr;

		return (*it_found).get();
	}

	std::expected<entity_type, std::error_code> map::get_entity_type(entity_id id) const
	{		
		map_entity const* e = find_entity(id);
		if (e != nullptr)
			return e->get_type();
		else
			return std::unexpected(std::make_error_code(std::errc::invalid_argument));
	}
}
