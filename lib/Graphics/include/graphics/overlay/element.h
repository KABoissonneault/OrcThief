#pragma once

#include <cstddef>

namespace ot::graphics::overlay
{
	class element; 
	class container;

	namespace detail
	{
		void init_element_impl(element&, void*) noexcept;
		void const* get_element_impl(element const&) noexcept;
	}

	// The base of all overlay elements that can be displayed on a surface
	class element
	{
		void* pimpl;

		friend void detail::init_element_impl(element&, void*) noexcept;
		friend void const* detail::get_element_impl(element const&) noexcept;

		void destroy_element() noexcept;

	public:
		element() noexcept;
		element(element const&) = delete;
		element(element&&) noexcept;
		element& operator=(element const&) = delete;
		element& operator=(element&&) noexcept;
		~element();

		void show();
		void hide();
		[[nodiscard]] bool is_visible() const noexcept;

		// Sets the position relative to the parent container, in normalised [0.0, 1.0] coordinates representing the screen size
		void set_position(double left, double top);

		container* get_parent() noexcept;
	};
}