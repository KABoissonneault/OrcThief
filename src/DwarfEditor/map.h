#pragma once

#include "map.fwd.h"

#include "egfx/mesh_definition.h"
#include "egfx/node/mesh.h"

#include "math/transform_matrix.h"

#include <vector>
#include <span>
#include <memory>

namespace ot::dedit
{
	enum class entity_id : uint64_t {};

	struct brush
	{
		entity_id id;
		std::shared_ptr<egfx::mesh_definition const> mesh_def;
		egfx::node::mesh node;

		math::transform_matrix get_local_transform() const noexcept;
		math::transform_matrix get_world_transform(math::transform_matrix const& parent) const noexcept;
		
		entity_id get_id() const noexcept { return id; }

		void reload_node();
		void reload_node(std::shared_ptr<egfx::mesh_definition const> new_def);
	};

	class map
	{
		uint64_t next_entity_id = 0;
		std::vector<brush> brushes;
		egfx::node::object_ref root_node;

	public:
		map(egfx::node::object_ref root_node);

		entity_id allocate_entity_id();
		brush& make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id);
		void delete_brush(entity_id id);

		void clear();

		std::span<brush> get_brushes() noexcept { return brushes; }
		std::span<brush const> get_brushes() const noexcept { return brushes; }
		brush const& get_brush(size_t i) const { return brushes[i]; }
		brush const* find_brush(entity_id id) const noexcept;
		brush* find_brush(entity_id id) noexcept;
	};
}
