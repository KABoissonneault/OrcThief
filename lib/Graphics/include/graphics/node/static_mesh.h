#pragma once

#include "graphics/node/static_mesh.fwd.h"
#include "graphics/node/object.h"

#include <string>

namespace ot::graphics
{
	class scene;
	class mesh_definition;

	namespace node
	{
		namespace detail
		{
			void init_static_mesh_impl(static_mesh&, void*, void*) noexcept;
			void* get_mesh_ptr_impl(static_mesh&) noexcept;
		}

		class static_mesh : public object
		{
			alignas(void*) std::byte storage_mesh[2 * sizeof(void*)];

			friend void detail::init_static_mesh_impl(static_mesh&, void*, void*) noexcept;
			friend void* detail::get_mesh_ptr_impl(static_mesh&) noexcept;

			void destroy_mesh() noexcept;
		public:
			static_mesh() noexcept;
			static_mesh(static_mesh const&) = delete;
			static_mesh(static_mesh&&) noexcept;
			static_mesh& operator=(static_mesh const&) = delete;
			static_mesh& operator=(static_mesh&&) noexcept;
			~static_mesh();
		};

		[[nodiscard]] static_mesh create_static_mesh(object& parent, std::string const& name, mesh_definition const& mesh);
		[[nodiscard]] static_mesh create_static_wireframe_mesh(object& parent, std::string const& name, mesh_definition const& mesh);
	}
}
