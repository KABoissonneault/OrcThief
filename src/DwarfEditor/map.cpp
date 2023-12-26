#include "map.h"

#include "egfx/node/object.h"
#include "egfx/node/mesh.h"

#include "serialize/serialize_mesh_definition.h"
#include "serialize/serialize_math.h"

#include <format>

namespace ot::dedit
{
	namespace
	{
		std::string make_brush_name(entity_id id)
		{
			return std::format("Brush {}", as_int(id));
		}
	}

	map_entity_iterator::map_entity_iterator(egfx::node::object_iterator it, egfx::node::object_iterator end) noexcept
		: node_iterator(it)
		, node_end(end)
	{

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
		egfx::node::object_ref node = *node_iterator;
		return *static_cast<map_entity*>(node.get_user_ptr());
	}

	auto map_entity_iterator::operator->() const -> pointer
	{
		egfx::node::object_ref node = *node_iterator;
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

	map_entity_range::map_entity_range(egfx::node::object_range node_range) noexcept
		: it(node_range.begin(), node_range.end())
	{

	}

	map_entity_const_iterator::map_entity_const_iterator(egfx::node::object_const_iterator it, egfx::node::object_const_iterator end) noexcept
		: node_iterator(it)
		, node_end(end)
	{

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
		egfx::node::object_cref node = *node_iterator;
		return *static_cast<map_entity const*>(node.get_user_ptr());
	}

	auto map_entity_const_iterator::operator->() const -> pointer
	{
		egfx::node::object_cref node = *node_iterator;
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

	map_entity_const_range::map_entity_const_range(egfx::node::object_const_range range) noexcept
		: it(range.begin(), range.end())
	{

	}

	map_entity::map_entity(entity_id id)
		: id(id)
	{

	}

	map_entity const* map_entity::get_parent() const noexcept
	{
		egfx::node::object_cref const node = get_node();
		auto const parent = node.get_parent();
		if (!parent)
			return nullptr;
		return static_cast<map_entity const*>(parent->get_user_ptr());
	}

	map_entity* map_entity::get_parent() noexcept
	{
		egfx::node::object_ref const node = get_node();
		auto const parent = node.get_parent();
		if (!parent)
			return nullptr;
		return static_cast<map_entity*>(parent->get_user_ptr());
	}

	map_entity_const_range map_entity::get_children() const noexcept
	{
		egfx::node::object_cref const node = get_node();
		egfx::node::object_const_range const child_objects = node.get_children();
		egfx::node::object_const_iterator it = child_objects.begin();
		egfx::node::object_const_iterator const end = child_objects.end();
		while (it != end && it->get_user_ptr() == nullptr)
			++it;
		return map_entity_const_range(egfx::node::object_const_range(it, end));
	}

	map_entity_range map_entity::get_children() noexcept
	{
		egfx::node::object_ref const node = get_node();
		egfx::node::object_range const child_objects = node.get_children();
		egfx::node::object_iterator it = child_objects.begin();
		egfx::node::object_iterator const end = child_objects.end();
		while (it != end && it->get_user_ptr() == nullptr)
			++it;
		return map_entity_range(egfx::node::object_range(it, end));
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
		egfx::node::object_cref const node = get_node();
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
		egfx::node::object_ref const node = get_node();

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

	root_entity::root_entity(entity_id id, egfx::node::object_ref node)
		: map_entity(id)
		, node(node)
	{
		node.set_user_ptr(this);
	}

	brush::brush(entity_id id)
		: map_entity(id)
	{

	}

	brush::brush(entity_id id, map_entity& parent, std::shared_ptr<egfx::mesh_definition const> mesh_def)
		: map_entity(id)
		, mesh_def(mesh_def)
		, mesh(egfx::node::create_mesh(parent.get_node(), make_brush_name(id), *mesh_def))
	{
		mesh.set_user_ptr(this);
	}

	bool brush::fwrite(std::FILE* f) const
	{
		if (!serialize::fwrite(*mesh_def, f))
			return false;

		if (!serialize::fwrite(mesh.get_position(), f)
			|| !serialize::fwrite(mesh.get_rotation(), f)
			|| !serialize::fwrite(mesh.get_scale(), f))
			return false;

		return true;
	}

	bool brush::fread(map_entity& parent, std::FILE* f)
	{
		auto read_mesh_def = std::make_shared<egfx::mesh_definition>();
		if (!serialize::fread(*read_mesh_def, f))
			return false;
				
		math::point3f position;
		math::quaternion rotation;
		math::scales scales;
		if (!serialize::fread(position, f) || !serialize::fread(rotation, f) || !serialize::fread(scales, f))
			return false;

		mesh_def = std::move(read_mesh_def);
		mesh = egfx::node::create_mesh(parent.get_node(), make_brush_name(get_id()), *mesh_def);

		mesh.set_position(position);
		mesh.set_rotation(rotation);
		mesh.set_scale(scales);

		mesh.set_user_ptr(this);

		return true;
	}

	void brush::reload_node(std::shared_ptr<egfx::mesh_definition const> new_def)
	{
		mesh_def = std::move(new_def);
		mesh.reload_mesh(*mesh_def);
	}

	map::map(egfx::node::object_ref root_node)
		: root(allocate_entity_id(), root_node)
	{

	}

	entity_id map::allocate_entity_id()
	{
		return static_cast<entity_id>(next_entity_id++);
	}

	brush& map::make_default_brush(entity_id id)
	{
		brushes.push_back(ot::make_unique<brush>(id));
		return *brushes.back();
	}

	brush& map::make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id)
	{
		brushes.push_back(ot::make_unique<brush>(id, root, mesh_def));
		return *brushes.back();
	}

	brush& map::make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id, map_entity& parent)
	{
		brushes.push_back(ot::make_unique<brush>(id, parent, mesh_def));
		return *brushes.back();
	}

	void map::delete_brush(entity_id id)
	{
		auto const it_found = std::find_if(brushes.begin(), brushes.end(), [id](uptr<brush> const& b) { return b->get_id() == id; });
		if (it_found != brushes.end())
		{
			brushes.erase(it_found);
		}
	}

	void map::clear()
	{
		brushes.clear();
		next_entity_id = 1; // Root always has id 0
	}

	brush const* map::find_brush(entity_id id) const noexcept
	{
		auto const it_found = std::find_if(brushes.begin(), brushes.end(), [id](uptr<brush> const& b) { return b->get_id() == id; });
		return it_found != brushes.end() ? it_found->get() : nullptr;
	}

	brush* map::find_brush(entity_id id) noexcept
	{
		return const_cast<brush*>(static_cast<map const*>(this)->find_brush(id));
	}

	map_entity_const_range map::get_root_entities() const
	{
		return root.get_children();
	}

	map_entity* map::find_entity(entity_id id)
	{
		return root.find_recursive(id);
	}

	map_entity const* map::find_entity(entity_id id) const
	{
		return root.find_recursive(id);
	}

	std::expected<entity_type_t, std::error_code> map::get_entity_type(entity_id id) const
	{		
		map_entity const* e = find_entity(id);
		if (e != nullptr)
			return e->get_type();
		else
			return std::unexpected(std::make_error_code(std::errc::invalid_argument));
	}
}
