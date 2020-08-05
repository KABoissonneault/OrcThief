#pragma once

#include <cstddef>
#include <string>

namespace ot::graphics::overlay
{
	class container;
	class surface;

	namespace detail
	{
		void init_surface_impl(surface&, void*) noexcept;
		void const* get_surface_impl(surface const&) noexcept;
	}

	// 2d overlay that renders elements
	class surface
	{
		void* pimpl;

		friend void detail::init_surface_impl(surface&, void*) noexcept;
		friend void const* detail::get_surface_impl(surface const&) noexcept;
	public:
		void add(container& c);

		// Makes the surface visible on the scene currently displaying the overlay
		void show();
		void hide();
		[[nodiscard]] bool is_visible() const noexcept;
	};

	[[nodiscard]] surface create_surface(std::string const& name);
}
