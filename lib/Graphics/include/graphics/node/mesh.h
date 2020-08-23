#pragma once

#include "graphics/mesh_definition.fwd.h"
#include "graphics/node/mesh.fwd.h"
#include "graphics/node/object.h"

#include <string>

namespace ot::graphics::node
{
	namespace detail
	{
		void init_mesh_impl(mesh&, void*, void*) noexcept;
		void const* get_mesh_ptr_impl(mesh const&) noexcept;
	}

	class mesh : public object
	{
		alignas(void*) std::byte storage_mesh[2 * sizeof(void*)];

		friend void detail::init_mesh_impl(mesh&, void*, void*) noexcept;
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

	[[nodiscard]] mesh create_mesh(object_ref parent, std::string const& name, mesh_definition const& mesh);
}
