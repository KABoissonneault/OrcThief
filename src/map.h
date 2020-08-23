#pragma once

#include "graphics/mesh_definition.h"
#include "graphics/node/mesh.h"

#include "math/transformation.h"

#include <vector>
#include <span>
#include <memory>

namespace ot
{
	enum class entity_id : uint32_t {};

	struct brush
	{
		std::shared_ptr<graphics::mesh_definition const> mesh_def;
		ot::graphics::node::mesh node;

		math::transformation get_local_transform() const noexcept;
		math::transformation get_world_transform(math::transformation const& parent) const noexcept;
		entity_id get_id() const noexcept;

		void reload_node();
		void reload_node(std::shared_ptr<graphics::mesh_definition const> new_def);
	};

	class map
	{
		std::vector<brush> brushes;

	public:
		void add_brush(brush b);

		std::span<brush> get_brushes() noexcept { return brushes; }
		std::span<brush const> get_brushes() const noexcept { return brushes; }
		brush const* find_brush(entity_id id) const noexcept;
		brush* find_brush(entity_id id) noexcept;
	};
}
