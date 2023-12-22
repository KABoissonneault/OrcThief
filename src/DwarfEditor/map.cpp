#include "map.h"

#include "egfx/node/object.h"
#include "egfx/node/mesh.h"

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

	map_entity::map_entity(entity_id id)
		: id(id)
	{

	}

	math::transform_matrix brush::get_local_transform() const noexcept
	{
		return math::transform_matrix::from_components(vector_from_origin(mesh.get_position()), mesh.get_rotation(), mesh.get_scale());
	}

	math::transform_matrix brush::get_world_transform(math::transform_matrix const& parent) const noexcept
	{
		return parent * get_local_transform();
	}

	brush::brush(entity_id id, std::shared_ptr<egfx::mesh_definition const> mesh_def, egfx::node::object_ref parent)
		: map_entity(id)
		, mesh_def(mesh_def)
		, mesh(egfx::node::create_mesh(parent, make_brush_name(id), *mesh_def))
	{
		
	}

	void brush::reload_node(std::shared_ptr<egfx::mesh_definition const> new_def)
	{
		mesh_def = std::move(new_def);
		mesh.reload_mesh(*mesh_def);
	}

	map::map(egfx::node::object_ref root_node)
		: root_node(root_node)
	{

	}

	entity_id map::allocate_entity_id()
	{
		return static_cast<entity_id>(next_entity_id++);
	}

	brush& map::make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id)
	{
		return brushes.emplace_back(id, mesh_def, root_node);
	}

	void map::delete_brush(entity_id id)
	{
		auto const it_found = std::find_if(brushes.begin(), brushes.end(), [id](brush const& b) { return b.get_id() == id; });
		if (it_found != brushes.end())
		{
			brushes.erase(it_found);
		}
	}

	math::transform_matrix map::get_brush_root_world_transform() const
	{
		// TODO: support composed maps, with different roots!
		return math::transform_matrix::identity();
	}

	void map::clear()
	{
		brushes.clear();
		next_entity_id = 0;
	}

	brush const* map::find_brush(entity_id id) const noexcept
	{
		auto const it_found = std::find_if(brushes.begin(), brushes.end(), [id](brush const& b) { return b.get_id() == id; });
		return it_found != brushes.end() ? std::to_address(it_found) : nullptr;
	}

	brush* map::find_brush(entity_id id) noexcept
	{
		return const_cast<brush*>(static_cast<map const*>(this)->find_brush(id));
	}
}
