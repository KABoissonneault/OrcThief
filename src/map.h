#pragma once

#include "graphics/mesh_definition.h"
#include "graphics/node/static_mesh.h"

#include "math/transformation.h"

#include <vector>
#include <span>

namespace ot
{
	enum class entity_id : uint32_t {};

	struct brush
	{
		graphics::mesh_definition mesh_def;
		ot::graphics::node::static_mesh node;

		math::transformation get_local_transform() const noexcept;
		math::transformation get_world_transform(math::transformation const& parent) const noexcept;
		entity_id get_id() const noexcept;
	};

	class map
	{
		std::vector<brush> brushes;

	public:
		void add_brush(brush b);

		std::span<brush> get_brushes() noexcept { return brushes; }
		std::span<brush const> get_brushes() const noexcept { return brushes; }
	};
}