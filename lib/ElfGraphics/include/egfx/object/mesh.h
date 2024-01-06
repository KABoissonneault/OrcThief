#pragma once

#include "egfx/object/mesh.fwd.h"

#include "egfx/mesh_definition.fwd.h"
#include "egfx/object/object.h"
#include "egfx/material.h"

#include <string>

namespace ot::egfx
{
	namespace detail
	{
		void init_mesh_impl(mesh&, void*) noexcept;
		void const* get_mesh_ptr_impl(mesh const&) noexcept;

		item_ref make_item_ref(void*) noexcept;
		item_cref make_item_cref(void const*) noexcept;
	}
	
	class mesh
	{
		alignas(void*) std::byte storage_mesh[2 * sizeof(void*)];

		friend void detail::init_mesh_impl(mesh&, void*) noexcept;
		friend void const* detail::get_mesh_ptr_impl(mesh const&) noexcept;

		void destroy_mesh() noexcept;
	public:
		mesh() noexcept;
		mesh(mesh const&) = delete;
		mesh(mesh&&) noexcept;
		mesh& operator=(mesh const&) = delete;
		mesh& operator=(mesh&&) noexcept;
		~mesh();

		std::string const& get_mesh_name() const noexcept;
		void reload_mesh(mesh_definition const& mesh);
	};

	// An item is a graphics object under a node that wraps a shared mesh, with additional properties for the instance

	class item_ref : public object_ref
	{
		friend item_ref detail::make_item_ref(void*) noexcept;

	public:
		// Hides the base version
		[[nodiscard]] object_type get_object_type() const noexcept { return object_type::item; }

		// Gets the material of the first sub-item
		material_handle_t get_material() const;

		// Sets the material of the first sub-item
		void set_material(material_handle_t const& mat);
	};

	extern template item_ref ref_cast<item_ref>(object_ref);

	class item_cref : public object_cref
	{
		item_cref() = default;

		friend item_cref detail::make_item_cref(void const*) noexcept;

	public:
		item_cref(item_ref) noexcept;

		// Hides the base version
		[[nodiscard]] object_type get_object_type() const noexcept { return object_type::item; }

		// Gets the material of the first sub-item
		material_handle_t get_material() const;
	};

	extern template item_cref ref_cast<item_cref>(object_ref);

	[[nodiscard]] mesh create_mesh(std::string const& name, mesh_definition const& mesh);
	void add_item(node_ref owner, mesh const& m);
}
