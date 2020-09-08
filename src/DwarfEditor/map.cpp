#include "map.h"

#include "egfx/node/object.h"

namespace ot::dedit
{
	math::transformation brush::get_local_transform() const noexcept
	{
		return { vector_from_origin(node.get_position()), node.get_rotation(), node.get_scale() };
	}

	math::transformation brush::get_world_transform(math::transformation const& parent) const noexcept
	{
		return concatenate(parent, get_local_transform());
	}

	entity_id brush::get_id() const noexcept
	{
		// can't be bothered to give it a separate id now
		return static_cast<entity_id>(node.get_node_id());
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

	void map::add_brush(brush b)
	{
		brushes.push_back(std::move(b));
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
