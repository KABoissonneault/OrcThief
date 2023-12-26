#pragma once

#include "map.fwd.h"

#include "core/uptr.h"

#include "egfx/mesh_definition.h"
#include "egfx/node/mesh.h"
#include "egfx/node/object.h"

#include "math/transform_matrix.h"

#include <vector>
#include <memory>
#include <expected>
#include <system_error>

namespace ot::dedit
{
	enum class entity_id : uint64_t {};
	[[nodiscard]] inline constexpr uint64_t as_int(entity_id i) noexcept { return static_cast<uint64_t>(i); }

	enum class entity_type_t
	{
		root,
		brush,
	};

	class map_entity;

	struct map_entity_end_sentinel
	{

	};

	inline constexpr map_entity_end_sentinel map_entity_end;

	class map_entity_iterator
	{
		egfx::node::object_iterator node_iterator;
		egfx::node::object_iterator node_end;

		friend class map_entity_const_iterator;

	public:
		map_entity_iterator() = default;
		map_entity_iterator(egfx::node::object_iterator it, egfx::node::object_iterator end) noexcept;

		using difference_type = ptrdiff_t;
		using element_type = map_entity;
		using pointer = map_entity*;
		using reference = map_entity&;

		map_entity_iterator& operator++();
		[[nodiscard]] map_entity_iterator operator++(int);
		[[nodiscard]] reference operator*() const;
		[[nodiscard]] pointer operator->() const;

		[[nodiscard]] bool operator==(map_entity_iterator const& rhs) const noexcept;
		[[nodiscard]] bool operator==(map_entity_end_sentinel const& rhs) const noexcept;
	};

	class map_entity_range
	{
		map_entity_iterator it;

		friend class map_entity_const_range;

	public:
		map_entity_range() = default;
		map_entity_range(map_entity_iterator beg) noexcept;
		map_entity_range(egfx::node::object_range node_range) noexcept;

		[[nodiscard]] map_entity_iterator begin() const noexcept { return it; }
		[[nodiscard]] map_entity_end_sentinel end() const noexcept { return map_entity_end; }

		[[nodiscard]] bool empty() const noexcept { return it == end(); }
		[[nodiscard]] size_t size() const { return std::ranges::distance(it, end()); }
	};

	class map_entity_const_iterator
	{
		egfx::node::object_const_iterator node_iterator;
		egfx::node::object_const_iterator node_end;

	public:
		map_entity_const_iterator() = default;
		map_entity_const_iterator(egfx::node::object_const_iterator it, egfx::node::object_const_iterator end) noexcept;
		map_entity_const_iterator(map_entity_iterator it) noexcept;

		using difference_type = ptrdiff_t;
		using element_type = map_entity;
		using pointer = map_entity const*;
		using reference = map_entity const&;

		map_entity_const_iterator& operator++();
		[[nodiscard]] map_entity_const_iterator operator++(int);
		[[nodiscard]] reference operator*() const;
		[[nodiscard]] pointer operator->() const;

		[[nodiscard]] bool operator==(map_entity_const_iterator const& rhs) const noexcept;
		[[nodiscard]] bool operator==(map_entity_end_sentinel const& rhs) const noexcept;
	};

	static_assert(std::forward_iterator<map_entity_const_iterator>);

	class map_entity_const_range
	{
		map_entity_const_iterator it;

	public:
		map_entity_const_range() = default;
		map_entity_const_range(map_entity_const_iterator beg) noexcept;
		map_entity_const_range(map_entity_range range) noexcept;
		map_entity_const_range(egfx::node::object_const_range range) noexcept;

		[[nodiscard]] map_entity_const_iterator begin() const noexcept { return it; }
		[[nodiscard]] map_entity_end_sentinel end() const noexcept { return map_entity_end; }

		[[nodiscard]] bool empty() const noexcept { return it == end(); }
		[[nodiscard]] size_t size() const { return std::ranges::distance(it, end()); }
	};

	class map_entity
	{
		entity_id id;

	protected:
		map_entity(entity_id id);
		map_entity(map_entity&&) = delete;
		map_entity& operator=(map_entity&&) = delete;

	public:
		
		[[nodiscard]] entity_id get_id() const noexcept { return id; }
		[[nodiscard]] virtual egfx::node::object_ref get_node() noexcept = 0;
		[[nodiscard]] virtual egfx::node::object_cref get_node() const noexcept = 0;
		[[nodiscard]] virtual std::string_view get_name() const noexcept = 0;
		[[nodiscard]] virtual entity_type_t get_type() const noexcept = 0;
		[[nodiscard]] virtual bool fwrite(std::FILE* f) const = 0;
		[[nodiscard]] virtual bool fread(map_entity& parent, std::FILE* f) = 0;
		
		[[nodiscard]] map_entity const* get_parent() const noexcept;
		[[nodiscard]] map_entity* get_parent() noexcept;

		[[nodiscard]] map_entity_const_range get_children() const noexcept;
		[[nodiscard]] map_entity_range get_children() noexcept;

		template<typename Callback>
		bool for_each_recursive(Callback cb)
		{
			if (cb(*this))
				return true;

			for (map_entity& m : get_children())
				if (m.for_each_recursive(cb))
					return true;
			
			return false;
		}

		template<typename Callback>
		bool for_each_recursive(Callback cb) const
		{
			if (cb(*this))
				return true;

			for (map_entity const& m : get_children())
				if (m.for_each_recursive(cb))
					return true;

			return false;
		}

		[[nodiscard]] map_entity* find_recursive(entity_id id);
		[[nodiscard]] map_entity const* find_recursive(entity_id id) const;

		[[nodiscard]] math::transform_matrix get_local_transform() const noexcept;
		[[nodiscard]] math::transform_matrix get_world_transform() const noexcept;

		void set_world_transform(math::transform_matrix const& m) noexcept;
	};

	class root_entity final : public map_entity
	{
		egfx::node::object_ref node;

	public:
		root_entity(entity_id id, egfx::node::object_ref node);

		[[nodiscard]] virtual egfx::node::object_ref get_node() noexcept override { return node; }
		[[nodiscard]] virtual egfx::node::object_cref get_node() const noexcept override { return node; }
		[[nodiscard]] virtual std::string_view get_name() const noexcept override { return "Root"; }
		[[nodiscard]] virtual entity_type_t get_type() const noexcept override { return entity_type_t::root; }
		virtual bool fwrite(std::FILE*) const override { return true; }
		virtual bool fread(map_entity&, std::FILE*) override { return true; }
	};

	class brush final : public map_entity
	{
		std::shared_ptr<egfx::mesh_definition const> mesh_def;
		egfx::node::mesh mesh;

	public:
		brush(entity_id id);
		brush(entity_id id, map_entity& parent, std::shared_ptr<egfx::mesh_definition const> mesh_def);

		[[nodiscard]] egfx::mesh_definition const& get_mesh_def() const noexcept { return *mesh_def; }
		[[nodiscard]] std::shared_ptr<egfx::mesh_definition const> get_shared_mesh_def() const noexcept { return mesh_def; }
		[[nodiscard]] egfx::node::mesh& get_mesh_node() noexcept { return mesh; }
		[[nodiscard]] egfx::node::mesh const& get_mesh_node() const noexcept { return mesh; }

		[[nodiscard]] virtual egfx::node::object_ref get_node() noexcept override { return mesh; }
		[[nodiscard]] virtual egfx::node::object_cref get_node() const noexcept override { return mesh; }		
		[[nodiscard]] virtual std::string_view get_name() const noexcept override { return mesh.get_mesh_name(); }
		[[nodiscard]] virtual entity_type_t get_type() const noexcept override { return entity_type_t::brush; }
		[[nodiscard]] virtual bool fwrite(std::FILE*) const override;
		[[nodiscard]] virtual bool fread(map_entity& parent, std::FILE*) override;

		void reload_node(std::shared_ptr<egfx::mesh_definition const> new_def);
	};

	class map
	{
		uint64_t next_entity_id = 0;
		std::vector<uptr<brush>> brushes;
		root_entity root;

	public:
		map(egfx::node::object_ref root_node);
				
		root_entity& get_root() noexcept { return root; }

		[[nodiscard]] entity_id allocate_entity_id();

		brush& make_default_brush(entity_id id);
		brush& make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id);
		brush& make_brush(std::shared_ptr<egfx::mesh_definition const> mesh_def, entity_id id, map_entity& parent);
		void delete_brush(entity_id id);

		void clear();

		[[nodiscard]] brush const* find_brush(entity_id id) const noexcept;
		[[nodiscard]] brush* find_brush(entity_id id) noexcept;

		[[nodiscard]] map_entity_range get_root_entities() { return root.get_children(); }
		[[nodiscard]] map_entity_const_range get_root_entities() const;

		[[nodiscard]] map_entity* find_entity(entity_id id);
		[[nodiscard]] map_entity const* find_entity(entity_id id) const;
		bool has_entity(entity_id id) const { return find_entity(id) != nullptr; }

		std::expected<entity_type_t, std::error_code> get_entity_type(entity_id id) const;
	};
}
