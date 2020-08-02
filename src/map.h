#pragma once

#include "graphics/mesh_definition.h"
#include "graphics/node/static_mesh.h"

#include <vector>
#include <span>

namespace ot
{
	struct brush
	{
		graphics::mesh_definition mesh_def;
		ot::graphics::node::static_mesh node;
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