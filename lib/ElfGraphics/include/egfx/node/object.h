#pragma once

#include "egfx/node/object.fwd.h"

#include "core/size_t.h"
#include "core/iterator/arrow_proxy.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <optional>

namespace ot::egfx::node
{
	class object_cref;
	class object_ref;

	namespace detail
	{
		object_cref make_object_cref(void const*) noexcept;
		object_ref make_object_ref(void*) noexcept;
		void const* get_object_impl(object_cref) noexcept;
		void* get_object_impl(object_ref) noexcept;

		template<typename Derived>
		class object_const_impl
		{
			using derived = Derived;
		public:
			// Id of the node itself
			[[nodiscard]] node_id get_node_id() const noexcept;

			// Returns true if the node contains a sub-object with the given id
			[[nodiscard]] bool contains(object_id id) const noexcept;

			// Gets the local position of the node relative to its parent
			[[nodiscard]] math::point3f get_position() const noexcept;

			// Gets the local rotation of the node
			[[nodiscard]] math::quaternion get_rotation() const noexcept;

			// Gets the local scale of the node
			[[nodiscard]] math::scales get_scale() const noexcept;

			// Gets the user pointer of the node
			[[nodiscard]] void* get_user_ptr() const noexcept;

			// Gets the number of children to this object
			[[nodiscard]] size_t get_child_count() const noexcept;
		};
	}
	
	class object_range;
	class object_const_range;

	class object_cref : public detail::object_const_impl<object_cref>
	{
		void const* pimpl;

		friend object_cref detail::make_object_cref(void const*) noexcept;
		friend void const* detail::get_object_impl(object_cref) noexcept;

		friend class detail::object_const_impl<object_cref>;

	public:
		// Gets the node of the parent
		std::optional<object_cref> get_parent() const noexcept;

		// Gets the specified child, if any
		std::optional<object_cref> get_child(size_t i) const;

		// Gets the range of children
		object_const_range get_children() const noexcept;
	};
	
	class object_ref : public detail::object_const_impl<object_ref>
	{
		void* pimpl;

		friend object_ref detail::make_object_ref(void*) noexcept;
		friend void* detail::get_object_impl(object_ref) noexcept;

	public:
		operator object_cref() const noexcept { return detail::make_object_cref(pimpl); }

		// Sets the local position of the node relative to its parent
		void set_position(math::point3f position) const noexcept;

		// Sets the rotation of the node
		void set_rotation(math::quaternion rot) const noexcept;
		// Sets the rotation of the node so that it faces in the direction of the vector
		void set_direction(math::vector3f direction) const noexcept;
		// Changes the rotation of the node by 'rad' radians around the given vector
		void rotate_around(math::vector3f axis, float rad) const noexcept;

		// Set the scale of the node
		void set_scale(float s) const noexcept;
		void set_scale(math::scales s) const noexcept;

		// Sets the input node as a child of this node.
		void attach_child(object_ref child) const noexcept;

		// Gets the node of the parent
		[[nodiscard]] std::optional<object_ref> get_parent() const noexcept;

		// Gets the node of the specified child
		[[nodiscard]] std::optional<object_ref> get_child(size_t i) const;

		// Gets the range of children
		[[nodiscard]] object_range get_children() const noexcept;

		// Sets a user pointer (ie: application wrapper) in the node
		void set_user_ptr(void* user_ptr) const;
	};

	class object : public detail::object_const_impl<object>
	{
		void* pimpl;
		void destroy_node() noexcept;
		
	protected:
		void set_impl(object_ref r) noexcept;

	public:
		object() noexcept;
		object(object const&) = delete;
		object(object&&) noexcept;
		object& operator=(object const&) = delete;
		object& operator=(object&&) noexcept;
		~object();

		operator object_cref() const noexcept { return detail::make_object_cref(pimpl); }
		operator object_ref() noexcept { return detail::make_object_ref(pimpl); }

		// Sets the local position of the node relative to its parent
		void set_position(math::point3f position) noexcept;

		// Sets the rotation of the node
		void set_rotation(math::quaternion rot) noexcept;
		// Sets the rotation of the node so that it faces in the direction of the vector
		void set_direction(math::vector3f direction) noexcept;
		// Changes the rotation of the node by 'rad' radians around the given vector
		void rotate_around(math::vector3f axis, float rad) noexcept;

		// Set the scale of the node
		void set_scale(float s) noexcept;
		void set_scale(math::scales s) noexcept;

		// Sets the input node as a child of this node.
		void attach_child(object_ref child) noexcept;

		// Gets the node of the parent
		[[nodiscard]] std::optional<object_cref> get_parent() const noexcept;
		[[nodiscard]] std::optional<object_ref> get_parent() noexcept;

		// Gets the node of the specified child
		[[nodiscard]] std::optional<object_cref> get_child(size_t i) const;
		[[nodiscard]] std::optional<object_ref> get_child(size_t i);

		// Gets the range of children
		[[nodiscard]] object_const_range get_children() const noexcept;
		[[nodiscard]] object_range get_children() noexcept;

		// Sets a user pointer (ie: application wrapper) in the node
		void set_user_ptr(void* user_ptr);
	};

	extern template class detail::object_const_impl<object_cref>;
	extern template class detail::object_const_impl<object_ref>;
	extern template class detail::object_const_impl<object>;

	class object_iterator
	{
		void* scene_node = nullptr;

		object_iterator(void* node) noexcept;

		friend class object_ref;
		friend class object;
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
		void const* scene_node = nullptr;

		object_const_iterator(void const* node) noexcept;

		friend class object_cref;
		friend class object_ref;
		friend class object;

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
