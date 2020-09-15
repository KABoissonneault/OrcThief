#pragma once

#include "egfx/node/object.fwd.h"

#include "math/vector3.h"
#include "math/quaternion.h"

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
			[[nodiscard]] float get_scale() const noexcept;
		};
	}

	class object_cref : public detail::object_const_impl<object_cref>
	{
		void const* pimpl;

		friend object_cref detail::make_object_cref(void const*) noexcept;
		friend void const* detail::get_object_impl(object_cref) noexcept;

		friend class detail::object_const_impl<object_cref>;

	public:
		// Get the node of the parent
		object_cref get_parent() const noexcept;
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

		// Sets the rotation of the node so that it faces in the direction of the vector
		void set_direction(math::vector3f direction) const noexcept;
		// Changes the rotation of the node by 'rad' radians around the given vector
		void rotate_around(math::vector3f axis, float rad) const noexcept;

		// Sets the input node as a child of this node.
		void attach_child(object_ref child) const noexcept;

		// Get the node of the parent
		object_ref get_parent() const noexcept;
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

		// Sets the rotation of the node so that it faces in the direction of the vector
		void set_direction(math::vector3f direction) noexcept;
		// Changes the rotation of the node by 'rad' radians around the given vector
		void rotate_around(math::vector3f axis, float rad) noexcept;

		// Sets the input node as a child of this node.
		void attach_child(object_ref child) noexcept;

		// Get the node of the parent
		object_cref get_parent() const noexcept;
		// Get the node of the parent
		object_ref get_parent() noexcept;
	};

	extern template class detail::object_const_impl<object_cref>;
	extern template class detail::object_const_impl<object_ref>;
	extern template class detail::object_const_impl<object>;
}
