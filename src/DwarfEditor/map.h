#pragma once

#include "egfx/mesh_definition.h"
#include "egfx/node/mesh.h"

#include "math/transform_matrix.h"

#include <vector>
#include <span>
#include <memory>

namespace ot::dedit
{
	enum class entity_id : uint32_t {};

	struct brush
	{
		std::shared_ptr<egfx::mesh_definition const> mesh_def;
		egfx::node::mesh node;

		math::transform_matrix get_local_transform() const noexcept;
		math::transform_matrix get_world_transform(math::transform_matrix const& parent) const noexcept;
		
		entity_id get_id() const noexcept;

		void reload_node();
		void reload_node(std::shared_ptr<egfx::mesh_definition const> new_def);
	};

	class map
	{
		std::vector<brush> brushes;

	public:
		void add_brush(brush b);

		std::span<brush> get_brushes() noexcept { return brushes; }
		std::span<brush const> get_brushes() const noexcept { return brushes; }
		brush const& get_brush(size_t i) const { return brushes[i]; }
		brush const* find_brush(entity_id id) const noexcept;
		brush* find_brush(entity_id id) noexcept;
	};
}
