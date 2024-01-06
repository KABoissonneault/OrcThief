#pragma once

#include "map.fwd.h"

#include "core/uptr.h"
#include "core/directive.h"

#include "egfx/mesh_definition.h"
#include "egfx/object/mesh.h"
#include "egfx/node.h"

#include "math/transform_matrix.h"

#include <cassert>
#include <vector>
#include <memory>
#include <expected>
#include <system_error>

namespace ot::dedit
{	
	[[nodiscard]] inline constexpr uint64_t as_int(entity_id i) noexcept { return static_cast<uint64_t>(i); }
	[[nodiscard]] constexpr std::string_view as_string(entity_type t) noexcept
	{
		switch (t)
		{
		case entity_type::root: return "Root";
		case entity_type::brush: return "Brush";
		default: assert(false); OT_UNREACHABLE();
		}
	}


	struct map_entity_end_sentinel
	{

	};

	inline constexpr map_entity_end_sentinel map_entity_end;

	class map_entity_iterator
	{
		egfx::node_iterator node_iterator;
		egfx::node_iterator node_end;

		friend class map_entity_const_iterator;

	public:
		map_entity_iterator() = default;
		map_entity_iterator(egfx::node_iterator it, egfx::node_iterator end) noexcept;

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
		map_entity_range(egfx::node_range node_range) noexcept;

		[[nodiscard]] map_entity_iterator begin() const noexcept { return it; }
		[[nodiscard]] map_entity_end_sentinel end() const noexcept { return map_entity_end; }

		[[nodiscard]] bool empty() const noexcept { return it == end(); }
		[[nodiscard]] size_t size() const { return std::ranges::distance(it, end()); }
	};

	class map_entity_const_iterator
	{
		egfx::node_const_iterator node_iterator;
		egfx::node_const_iterator node_end;

	public:
		map_entity_const_iterator() = default;
		map_entity_const_iterator(egfx::node_const_iterator it, egfx::node_const_iterator end) noexcept;
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
		map_entity_const_range(egfx::node_const_range range) noexcept;

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
		virtual ~map_entity() = default;

		[[nodiscard]] entity_id get_id() const noexcept { return id; }
		[[nodiscard]] virtual egfx::node_ref get_node() noexcept = 0;
		[[nodiscard]] virtual egfx::node_cref get_node() const noexcept = 0;
		[[nodiscard]] virtual std::string_view get_name() const noexcept = 0;
		[[nodiscard]] virtual entity_type get_type() const noexcept = 0;
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
		egfx::node_ref node;

	public:
		static constexpr entity_type type = entity_type::root;

		root_entity(entity_id id, egfx::node_ref node);

		[[nodiscard]] virtual egfx::node_ref get_node() noexcept override { return node; }
		[[nodiscard]] virtual egfx::node_cref get_node() const noexcept override { return node; }
		[[nodiscard]] virtual std::string_view get_name() const noexcept override { return "Root"; }
		[[nodiscard]] virtual entity_type get_type() const noexcept override { return type; }
		virtual bool fwrite(std::FILE*) const override { return true; }
		virtual bool fread(map_entity&, std::FILE*) override { return true; }
	};

	// Entities which own their own scene node
	class node_entity : public map_entity
	{
		egfx::node node;

	protected:
		node_entity(entity_id id);
		node_entity(entity_id id, map_entity& parent, std::string_view name);

	public:
		[[nodiscard]] virtual egfx::node_ref get_node() noexcept override final { return node; }
		[[nodiscard]] virtual egfx::node_cref get_node() const noexcept override final { return node; }
		[[nodiscard]] virtual std::string_view get_name() const noexcept override final { return node.get_name(); }
		[[nodiscard]] virtual bool fwrite(std::FILE* file) const override;
		[[nodiscard]] virtual bool fread(map_entity& parent, std::FILE* file) override;
	};

	class brush_entity final : public node_entity
	{
		std::shared_ptr<egfx::mesh_definition const> mesh_def;
		egfx::mesh mesh;

	public:
		static constexpr entity_type type = entity_type::brush;

		brush_entity(entity_id id);
		brush_entity(entity_id id, map_entity& parent, std::shared_ptr<egfx::mesh_definition const> mesh_def);

		[[nodiscard]] egfx::mesh_definition const& get_mesh_def() const noexcept { return *mesh_def; }
		[[nodiscard]] std::shared_ptr<egfx::mesh_definition const> get_shared_mesh_def() const noexcept { return mesh_def; }

		[[nodiscard]] egfx::item_ref get_item() noexcept { return get_node().get_object(0).as<egfx::item_ref>(); }
		[[nodiscard]] egfx::item_cref get_item() const noexcept { return get_node().get_object(0).as<egfx::item_cref>(); }
				
		[[nodiscard]] virtual entity_type get_type() const noexcept override { return type; }
		[[nodiscard]] virtual bool fwrite(std::FILE* file) const override;
		[[nodiscard]] virtual bool fread(map_entity& parent, std::FILE* file) override;

		void reload_node(std::shared_ptr<egfx::mesh_definition const> new_def);
	};

	using brush = brush_entity; // not renaming everything for now

	class map
	{
		uint64_t next_entity_id = 0;
		std::vector<uptr<map_entity>> entities;
		root_entity root;

		void on_new_entity(entity_id id);

	public:
		map(egfx::node_ref root_node);
				
		root_entity& get_root() noexcept { return root; }

		[[nodiscard]] entity_id allocate_entity_id();

		template<typename... Args>
		map_entity& make_entity(entity_type type, entity_id id, Args&&... args)
		{
			switch (type)
			{
			case entity_type::root:
				throw std::invalid_argument("Cannot create root entities");

			case entity_type::brush:
				return make_entity<brush_entity>(id, ot::forward<Args>(args)...);

			default:
				assert(false);
				OT_UNREACHABLE();
			}
		}

		template<typename EntityType, typename... Args>
		EntityType& make_entity(entity_id id, Args&&... args)
		{
			on_new_entity(id);
			entities.push_back(ot::make_unique<EntityType>(id, ot::forward<Args>(args)...));
			return static_cast<EntityType&>(*entities.back());
		}

		void delete_entity(entity_id id);

		void clear();

		[[nodiscard]] brush const* find_brush(entity_id id) const noexcept;
		[[nodiscard]] brush* find_brush(entity_id id) noexcept;

		[[nodiscard]] map_entity_range get_root_entities() { return root.get_children(); }
		[[nodiscard]] map_entity_const_range get_root_entities() const;

		[[nodiscard]] map_entity* find_entity(entity_id id);
		[[nodiscard]] map_entity const* find_entity(entity_id id) const;
		[[nodiscard]] bool has_entity(entity_id id) const { return find_entity(id) != nullptr; }

		[[nodiscard]] std::expected<entity_type, std::error_code> get_entity_type(entity_id id) const;
	};
}
