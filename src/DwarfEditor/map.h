#pragma once

#include "map.fwd.h"

#include "egfx/mesh_definition.h"
#include "egfx/node/mesh.h"
#include "egfx/node/object.h"

#include "math/transform_matrix.h"

#include <vector>
#include <span>
#include <memory>

namespace ot::dedit
{
	enum class entity_id : uint64_t {};
	[[nodiscard]] inline constexpr uint64_t as_int(entity_id i) noexcept { return static_cast<uint64_t>(i); }

	class map_entity
	{
		entity_id id;

	protected:
		map_entity(entity_id id);

	public:
		[[nodiscard]] entity_id get_id() const noexcept { return id; }
		[[nodiscard]] virtual egfx::node::object_ref get_node() noexcept = 0;
		[[nodiscard]] virtual egfx::node::object_cref get_node() const noexcept = 0;
		[[nodiscard]] virtual std::string_view get_name() const noexcept = 0;
	};

	class brush : public map_entity
	{
		std::shared_ptr<egfx::mesh_definition const> mesh_def;
		egfx::node::mesh mesh;

	public:
		brush(entity_id id, std::shared_ptr<egfx::mesh_definition const> mesh_def, egfx::node::object_ref parent);

		[[nodiscard]] egfx::mesh_definition const& get_mesh_def() const noexcept { return *mesh_def; }
		[[nodiscard]] std::shared_ptr<egfx::mesh_definition const> get_shared_mesh_def() const noexcept { return mesh_def; }
		[[nodiscard]] egfx::node::mesh& get_mesh_node() noexcept { return mesh; }
		[[nodiscard]] egfx::node::mesh const& get_mesh_node() const noexcept { return mesh; }
		[[nodiscard]] virtual egfx::node::object_ref get_node() noexcept { return mesh; }
		[[nodiscard]] virtual egfx::node::object_cref get_node() const noexcept { return mesh; }

		[[nodiscard]] math::transform_matrix get_local_transform() const noexcept;
		[[nodiscard]] math::transform_matrix get_world_transform(math::transform_matrix const& parent) const noexcept;
		
		[[nodiscard]] std::string_view get_name() const noexcept { return mesh.get_mesh_name(); }

		void reload_node(std::shared_ptr<egfx::mesh_definition const> new_def);
	};

	class map
	{
		uint64_t next_entity_id = 0;
		std::vector<brush> brushes;
		egfx::node::object_ref root_node;

	public:
		map(egfx::node::object_ref root_node);

		[[nodiscard]] entity_id allocate_entity_id();
		brush& make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id);
		void delete_brush(entity_id id);
		// Returns the world transform of the root of the brushes (usually the origin of the map)
		[[nodiscard]] math::transform_matrix get_brush_root_world_transform() const;

		void clear();

		[[nodiscard]] std::span<brush> get_brushes() noexcept { return brushes; }
		[[nodiscard]] std::span<brush const> get_brushes() const noexcept { return brushes; }
		[[nodiscard]] brush const& get_brush(size_t i) const { return brushes[i]; }
		[[nodiscard]] brush const* find_brush(entity_id id) const noexcept;
		[[nodiscard]] brush* find_brush(entity_id id) noexcept;
	};
}
