#pragma once

#include "egfx/node.fwd.h"
#include "egfx/object/object.fwd.h"

#include "core/iterator/arrow_proxy.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/transform_matrix.h"

#include <optional>

namespace ot::egfx
{
	class node_cref;
	class node_ref;

	namespace detail
	{
		node_cref make_node_cref(void const*) noexcept;
		node_ref make_node_ref(void*) noexcept;
		void const* get_node_impl(node_cref) noexcept;
		void* get_node_impl(node_ref) noexcept;

		template<typename Derived>
		class node_const_impl
		{
			using derived = Derived;
		public:
			// Id of the node itself
			[[nodiscard]] node_id get_node_id() const noexcept;

			// Returns true if the node contains a sub-node with the given id
			[[nodiscard]] bool contains(object_id id) const noexcept;

			// Gets the local position of the node relative to its parent
			[[nodiscard]] math::point3f get_position() const noexcept;

			// Gets the local rotation of the node
			[[nodiscard]] math::quaternion get_rotation() const noexcept;

			// Gets the local scale of the node
			[[nodiscard]] math::scales get_scale() const noexcept;

			// Gets the user pointer of the node
			[[nodiscard]] void* get_user_ptr() const noexcept;

			// Gets the number of children to this node
			[[nodiscard]] size_t get_child_count() const noexcept;

			// Gets the number of graphics objects attached to this node
			[[nodiscard]] size_t get_object_count() const noexcept;
		};
	}

	class node_range;
	class node_const_range;

	class node_cref : public detail::node_const_impl<node_cref>
	{
		void const* pimpl;

		friend node_cref detail::make_node_cref(void const*) noexcept;
		friend void const* detail::get_node_impl(node_cref) noexcept;

		friend class detail::node_const_impl<node_cref>;

	public:
		// Gets the node of the parent
		[[nodiscard]] std::optional<node_cref> get_parent() const noexcept;

		// Gets the specified child, if any
		[[nodiscard]] node_cref get_child(size_t i) const;

		// Gets the range of children
		[[nodiscard]] node_const_range get_children() const noexcept;

		// Gets the specific object, if any
		[[nodiscard]] object_cref get_object(size_t i) const;

		// Gets the graphics objects attached to this node
		[[nodiscard]] object_const_range get_objects() const noexcept;
	};

	class node_ref : public detail::node_const_impl<node_ref>
	{
		void* pimpl;

		friend node_ref detail::make_node_ref(void*) noexcept;
		friend void* detail::get_node_impl(node_ref) noexcept;

	public:
		operator node_cref() const noexcept { return detail::make_node_cref(pimpl); }

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
		void attach_child(node_ref child) const noexcept;

		// Gets the node of the parent
		[[nodiscard]] std::optional<node_ref> get_parent() const noexcept;

		// Gets the node of the specified child
		[[nodiscard]] node_ref get_child(size_t i) const;

		// Gets the range of children
		[[nodiscard]] node_range get_children() const noexcept;

		// Gets the specific object, if any
		[[nodiscard]] object_ref get_object(size_t i) const;

		// Gets the graphics objects attached to this node
		[[nodiscard]] object_range get_objects() const noexcept;

		// Sets a user pointer (ie: application wrapper) in the node
		void set_user_ptr(void* user_ptr) const;
	};

	class node : public detail::node_const_impl<node>
	{
		void* pimpl;
		void destroy_node() noexcept;

		friend node create_child_node(node_ref parent);

	protected:
		void set_impl(node_ref r) noexcept;

	public:
		node() noexcept;
		node(node const&) = delete;
		node(node&&) noexcept;
		node& operator=(node const&) = delete;
		node& operator=(node&&) noexcept;
		~node();

		operator node_cref() const noexcept { return detail::make_node_cref(pimpl); }
		operator node_ref() noexcept { return detail::make_node_ref(pimpl); }

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
		void attach_child(node_ref child) noexcept;

		// Gets the node of the parent
		[[nodiscard]] std::optional<node_cref> get_parent() const noexcept;
		[[nodiscard]] std::optional<node_ref> get_parent() noexcept;

		// Gets the node of the specified child
		[[nodiscard]] std::optional<node_cref> get_child(size_t i) const;
		[[nodiscard]] std::optional<node_ref> get_child(size_t i);

		// Gets the range of children
		[[nodiscard]] node_const_range get_children() const noexcept;
		[[nodiscard]] node_range get_children() noexcept;

		// Gets the specific object, if any
		[[nodiscard]] object_cref get_object(size_t i) const;
		[[nodiscard]] object_ref get_object(size_t i) ;

		// Gets the graphics objects attached to this node
		[[nodiscard]] object_const_range get_objects() const noexcept;
		[[nodiscard]] object_range get_objects() noexcept;

		// Sets a user pointer (ie: application wrapper) in the node
		void set_user_ptr(void* user_ptr);
	};

	node create_child_node(node_ref parent);

	extern template class detail::node_const_impl<node_cref>;
	extern template class detail::node_const_impl<node_ref>;
	extern template class detail::node_const_impl<node>;

	class node_iterator
	{
		void* scene_node = nullptr;

		node_iterator(void* node) noexcept;

		friend class node_ref;
		friend class node;
		friend class node_const_iterator;

	public:
		node_iterator() = default;

		using difference_type = ptrdiff_t;
		using element_type = node_ref;
		using pointer = arrow_proxy<element_type>;
		using reference = element_type;

		node_iterator& operator++();
		[[nodiscard]] node_iterator operator++(int);
		node_iterator& operator+=(difference_type n);

		node_iterator& operator--();
		[[nodiscard]] node_iterator operator--(int);
		node_iterator& operator-=(difference_type n);
		[[nodiscard]] node_iterator operator-(difference_type n) const;
		[[nodiscard]] difference_type operator-(node_iterator const& rhs) const noexcept;

		[[nodiscard]] reference operator*() const;
		[[nodiscard]] pointer operator->() const;
		[[nodiscard]] reference operator[](difference_type n) const;

		[[nodiscard]] std::strong_ordering operator<=>(node_iterator const& rhs) const noexcept = default;
	};

	[[nodiscard]] node_iterator operator+(node_iterator it, node_iterator::difference_type n);
	[[nodiscard]] node_iterator operator+(node_iterator::difference_type n, node_iterator it);

	static_assert(std::random_access_iterator<node_iterator>);

	class node_range
	{
		node_iterator it;
		node_iterator sent;

	public:
		node_range() = default;
		node_range(node_iterator beg, node_iterator end) noexcept;

		[[nodiscard]] node_iterator begin() const noexcept { return it; }
		[[nodiscard]] node_iterator end() const noexcept { return sent; }

		[[nodiscard]] bool empty() const noexcept { return it == sent; }
		[[nodiscard]] size_t size() const { return sent - it; }
	};

	class node_const_iterator
	{
		void const* scene_node = nullptr;

		node_const_iterator(void const* node) noexcept;

		friend class node_cref;
		friend class node_ref;
		friend class node;

	public:
		node_const_iterator() = default;
		node_const_iterator(node_iterator it) noexcept;

		using difference_type = ptrdiff_t;
		using element_type = node_cref;
		using pointer = arrow_proxy<element_type const>;
		using reference = element_type;

		node_const_iterator& operator++();
		[[nodiscard]] node_const_iterator operator++(int);
		node_const_iterator& operator+=(difference_type n);

		node_const_iterator& operator--();
		[[nodiscard]] node_const_iterator operator--(int);
		node_const_iterator& operator-=(difference_type n);
		[[nodiscard]] node_const_iterator operator-(difference_type n) const;
		[[nodiscard]] difference_type operator-(node_const_iterator const& rhs) const noexcept;

		[[nodiscard]] element_type operator*() const;
		[[nodiscard]] pointer operator->() const;
		[[nodiscard]] reference operator[](difference_type n) const;

		[[nodiscard]] std::strong_ordering operator<=>(node_const_iterator const& rhs) const noexcept = default;
	};

	[[nodiscard]] node_const_iterator operator+(node_const_iterator it, node_const_iterator::difference_type n);
	[[nodiscard]] node_const_iterator operator+(node_const_iterator::difference_type n, node_const_iterator it);

	static_assert(std::random_access_iterator<node_const_iterator>);

	class node_const_range
	{
		node_const_iterator it;
		node_const_iterator sent;

	public:
		node_const_range() = default;
		node_const_range(node_const_iterator beg, node_const_iterator end) noexcept;
		node_const_range(node_range range) noexcept;

		[[nodiscard]] node_const_iterator begin() const noexcept { return it; }
		[[nodiscard]] node_const_iterator end() const noexcept { return sent; }

		[[nodiscard]] bool empty() const noexcept { return it == sent; }
		[[nodiscard]] size_t size() const { return sent - it; }
	};
}
