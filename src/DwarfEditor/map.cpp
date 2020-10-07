#include "map.h"

#include "egfx/node/object.h"
#include "egfx/node/mesh.h"

namespace ot::dedit
{
	math::transform_matrix brush::get_local_transform() const noexcept
	{
		return math::transform_matrix::from_components(vector_from_origin(node.get_position()), node.get_rotation(), node.get_scale());
	}

	math::transform_matrix brush::get_world_transform(math::transform_matrix const& parent) const noexcept
	{
		return parent * get_local_transform();
	}

	void brush::reload_node()
	{
		node.reload_mesh(*mesh_def);
	}

	void brush::reload_node(std::shared_ptr<egfx::mesh_definition const> new_def)
	{
		mesh_def = std::move(new_def);
		reload_node();
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
		brush& b = brushes.emplace_back();
		b.id = id;
		b.mesh_def = std::move(mesh_def);
		std::string const mesh_name = "Brush" + std::to_string(static_cast<std::underlying_type_t<entity_id>>(b.id));
		b.node = egfx::node::create_mesh(root_node, mesh_name, *b.mesh_def);
		return b;
	}

	void map::delete_brush(entity_id id)
	{
		auto const it_found = std::find_if(brushes.begin(), brushes.end(), [id](brush const& b) { return b.get_id() == id; });
		if (it_found != brushes.end())
		{
			brushes.erase(it_found);
		}
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
