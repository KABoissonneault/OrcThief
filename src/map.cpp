#include "map.h"

#include "graphics/node/object.h"

namespace ot
{
	math::transformation brush::get_local_transform() const noexcept
	{
		return { vector_from_origin(node.get_position()), node.get_rotation(), node.get_scale() };
	}

	math::transformation brush::get_world_transform(math::transformation const& parent) const noexcept
	{
		return concatenate(parent, get_local_transform());
	}

	void map::add_brush(brush b)
	{
		brushes.push_back(std::move(b));
	}
}