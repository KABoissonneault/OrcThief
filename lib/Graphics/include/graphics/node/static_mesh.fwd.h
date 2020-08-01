#pragma once

#include "graphics/node/object.fwd.h"

#include <cstddef>

namespace ot::graphics::node
{
	class static_mesh : public object
	{
		alignas(void*) std::byte storage_mesh[2 * sizeof(void*)];

		friend void init_static_mesh_impl(static_mesh&, void*, void*) noexcept;
		friend void* get_mesh_ptr_impl(static_mesh&) noexcept;

		void destroy_mesh() noexcept;
	public:
		static_mesh() noexcept;
		static_mesh(static_mesh const&) = delete;
		static_mesh(static_mesh&&) noexcept;
		static_mesh& operator=(static_mesh const&) = delete;
		static_mesh& operator=(static_mesh&&) noexcept;
		~static_mesh();
	};
}
