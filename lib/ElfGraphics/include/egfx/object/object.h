#pragma once

#include "egfx/object/object.fwd.h"
#include "egfx/node.fwd.h"

#include "core/size_t.h"
#include "core/iterator/arrow_proxy.h"
#include "core/directive.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <optional>
#include <string_view>

namespace ot::egfx
{
	class object_cref;
	class object_ref;

	namespace detail
	{
		void init_object_cref(object_cref&, void const*) noexcept;
		void init_object_ref(object_ref&, void*) noexcept;
		object_cref make_object_cref(void const*) noexcept;
		object_ref make_object_ref(void*) noexcept;
		void const* get_object_impl(object_cref) noexcept;
		void* get_object_impl(object_ref) noexcept;

		template<typename Derived>
		class object_const_impl
		{
			using derived = Derived;
		public:
			// Id of the object itself
			[[nodiscard]] object_id get_object_id() const noexcept;

			// Object type
			[[nodiscard]] object_type get_object_type() const noexcept;

			[[nodiscard]] bool is_a(object_type t) const noexcept;

			template<typename Derived>
			[[nodiscard]] bool is_a() const noexcept
			{
				return is_a(Derived::type);
			}

			// Gets whether the object casts shadows
			// For lights, this controls whether the light makes other objects cast shadows
			[[nodiscard]] bool is_casting_shadows() const noexcept;
		};
	}
	
	inline constexpr uint32_t as_int(object_id id) noexcept { return static_cast<uint32_t>(id); }
	inline constexpr std::string_view as_string(object_type t) noexcept
	{
		switch (t)
		{
		case object_type::item: return "Item";
		case object_type::camera: return "Camera";
		case object_type::light: return "Light";
		}

		OT_UNREACHABLE();
	}

	class object_range;
	class object_const_range;

	template<typename Derived>
	Derived ref_cast(object_cref ref);

	class object_cref : public detail::object_const_impl<object_cref>
	{
		void const* pimpl;
		
		friend void detail::init_object_cref(object_cref&, void const*) noexcept;
		friend object_cref detail::make_object_cref(void const*) noexcept;
		friend void const* detail::get_object_impl(object_cref) noexcept;

		friend class detail::object_const_impl<object_cref>;

	protected:
		object_cref() = default;

	public:
		template<typename Derived>
		Derived as() { return ref_cast<Derived>(*this); }

		// Gets the owning node
		[[nodiscard]] node_cref get_owner() const noexcept;
	};
	
	template<typename Derived>
	Derived ref_cast(object_ref ref);

	class object_ref : public detail::object_const_impl<object_ref>
	{
		void* pimpl;

		friend void detail::init_object_ref(object_ref&, void*) noexcept;
		friend object_ref detail::make_object_ref(void*) noexcept;
		friend void* detail::get_object_impl(object_ref) noexcept;

	protected:
		object_ref() = default;

	public:
		operator object_cref() const noexcept { return detail::make_object_cref(pimpl); }

		template<typename Derived>
		Derived as() { return ref_cast<Derived>(*this); }

		// Gets the owning node
		[[nodiscard]] node_ref get_owner() const noexcept;

		// Sets whether the object casts shadows
		// For light objects, this sets whether the light causes other objects to cast shadows
		void set_casting_shadows(bool new_value) const noexcept;
	};

	extern template class detail::object_const_impl<object_cref>;
	extern template class detail::object_const_impl<object_ref>;

	class object_iterator
	{
		void* object_ptr_it = nullptr;

		object_iterator(void* object_ptr_it) noexcept;

		friend class node_ref;
		friend class object_const_iterator;

	public:
		object_iterator() = default;

		using difference_type = ptrdiff_t;
		using element_type = object_ref;
		using pointer = arrow_proxy<element_type>;
		using reference = element_type;

		object_iterator& operator++();
		[[nodiscard]] object_iterator operator++(int);
		object_iterator& operator+=(difference_type n);
		
		object_iterator& operator--();
		[[nodiscard]] object_iterator operator--(int);
		object_iterator& operator-=(difference_type n);
		[[nodiscard]] object_iterator operator-(difference_type n) const;
		[[nodiscard]] difference_type operator-(object_iterator const& rhs) const noexcept;
		
		[[nodiscard]] reference operator*() const;
		[[nodiscard]] pointer operator->() const;
		[[nodiscard]] reference operator[](difference_type n) const;

		[[nodiscard]] std::strong_ordering operator<=>(object_iterator const& rhs) const noexcept = default;
	};

	[[nodiscard]] object_iterator operator+(object_iterator it, object_iterator::difference_type n);
	[[nodiscard]] object_iterator operator+(object_iterator::difference_type n, object_iterator it);

	static_assert(std::random_access_iterator<object_iterator>);

	class object_range
	{
		object_iterator it;
		object_iterator sent;

	public:
		object_range() = default;
		object_range(object_iterator beg, object_iterator end) noexcept;

		[[nodiscard]] object_iterator begin() const noexcept { return it; }
		[[nodiscard]] object_iterator end() const noexcept { return sent; }

		[[nodiscard]] bool empty() const noexcept { return it == sent; }
		[[nodiscard]] size_t size() const { return sent - it; }
	};

	class object_const_iterator
	{
		void const* object_ptr_it = nullptr;

		object_const_iterator(void const* node) noexcept;

		friend class node_cref;
		friend class node_ref;

	public:
		object_const_iterator() = default;
		object_const_iterator(object_iterator it) noexcept;

		using difference_type = ptrdiff_t;
		using element_type = object_cref;
		using pointer = arrow_proxy<element_type const>;
		using reference = element_type;

		object_const_iterator& operator++();
		[[nodiscard]] object_const_iterator operator++(int);
		object_const_iterator& operator+=(difference_type n);

		object_const_iterator& operator--();
		[[nodiscard]] object_const_iterator operator--(int);
		object_const_iterator& operator-=(difference_type n);
		[[nodiscard]] object_const_iterator operator-(difference_type n) const;
		[[nodiscard]] difference_type operator-(object_const_iterator const& rhs) const noexcept;

		[[nodiscard]] element_type operator*() const;
		[[nodiscard]] pointer operator->() const;
		[[nodiscard]] reference operator[](difference_type n) const;

		[[nodiscard]] std::strong_ordering operator<=>(object_const_iterator const& rhs) const noexcept = default;
	};
	
	[[nodiscard]] object_const_iterator operator+(object_const_iterator it, object_const_iterator::difference_type n);
	[[nodiscard]] object_const_iterator operator+(object_const_iterator::difference_type n, object_const_iterator it);

	static_assert(std::random_access_iterator<object_const_iterator>);

	class object_const_range
	{
		object_const_iterator it;
		object_const_iterator sent;

	public:
		object_const_range() = default;
		object_const_range(object_const_iterator beg, object_const_iterator end) noexcept;
		object_const_range(object_range range) noexcept;

		[[nodiscard]] object_const_iterator begin() const noexcept { return it; }
		[[nodiscard]] object_const_iterator end() const noexcept { return sent; }

		[[nodiscard]] bool empty() const noexcept { return it == sent; }
		[[nodiscard]] size_t size() const { return sent - it; }
	};

}
