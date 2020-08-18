#pragma once

#include "graphics/node/object.fwd.h"

#include "math/vector3.h"
#include "math/quaternion.h"

namespace ot::graphics::node
{
	class object_cref;
	class object_ref;

	namespace detail
	{
		object_cref make_object_cref(void const*) noexcept;
		object_ref make_object_ref(void*) noexcept;
		void const* get_object_impl(object_cref) noexcept;
		void* get_object_impl(object_ref) noexcept;
	}

	class object_cref
	{
		void const* pimpl;

		friend object_cref detail::make_object_cref(void const*) noexcept;
		friend void const* detail::get_object_impl(object_cref) noexcept;

	public:
		// Id of the node itself
		[[nodiscard]] node_id get_node_id() const noexcept;

		// Returns true if the node contains a sub-object with the given id
		[[nodiscard]] bool contains(object_id id) const noexcept;

		// Gets the local position of the node relative to its parent
		[[nodiscard]] math::point3d get_position() const noexcept;

		// Gets the local rotation of the node
		[[nodiscard]] math::quaternion get_rotation() const noexcept;

		// gets the local scale of the node
		[[nodiscard]] double get_scale() const noexcept;

	};
	
	class object_ref
	{
		void* pimpl;

		friend object_ref detail::make_object_ref(void*) noexcept;
		friend void* detail::get_object_impl(object_ref) noexcept;

	public:
		operator object_cref() const noexcept { return detail::make_object_cref(pimpl); }

		// Id of the node itself
		[[nodiscard]] node_id get_node_id() const noexcept;

		// Returns true if the node contains a sub-object with the given id
		[[nodiscard]] bool contains(object_id id) const noexcept;

		// Sets the local position of the node relative to its parent
		void set_position(math::point3d position) const noexcept;
		// Gets the local position of the node relative to its parent
		[[nodiscard]] math::point3d get_position() const noexcept;

		// Gets the local rotation of the node
		[[nodiscard]] math::quaternion get_rotation() const noexcept;
		// Sets the rotation of the node so that it faces in the direction of the vector
		void set_direction(math::vector3d direction) const noexcept;
		// Changes the rotation of the node by 'rad' radians around the given vector
		void rotate_around(math::vector3d axis, double rad) const noexcept;

		// gets the local scale of the node
		[[nodiscard]] double get_scale() const noexcept;

		// Sets the input node as a child of this node.
		void attach_child(object_ref child) const noexcept;
	};


	class object
	{
		void destroy_node() noexcept;
		
	protected:
		object_ref ref;

	public:
		object() noexcept;
		object(object const&) = delete;
		object(object&&) noexcept;
		object& operator=(object const&) = delete;
		object& operator=(object&&) noexcept;
		~object();

		operator object_cref() const noexcept { return ref; }
		operator object_ref() noexcept { return ref; }

		// Id of the node itself
		[[nodiscard]] node_id get_node_id() const noexcept;

		// Returns true if the node contains a sub-object with the given id
		[[nodiscard]] bool contains(object_id id) const noexcept;

		// Sets the local position of the node relative to its parent
		void set_position(math::point3d position) noexcept;
		// Gets the local position of the node relative to its parent
		[[nodiscard]] math::point3d get_position() const noexcept;

		// Gets the local rotation of the node
		[[nodiscard]] math::quaternion get_rotation() const noexcept;
		// Sets the rotation of the node so that it faces in the direction of the vector
		void set_direction(math::vector3d direction) noexcept;
		// Changes the rotation of the node by 'rad' radians around the given vector
		void rotate_around(math::vector3d axis, double rad) noexcept;

		// gets the local scale of the node
		[[nodiscard]] double get_scale() const noexcept;

		// Sets the input node as a child of this node.
		void attach_child(object_ref child) noexcept;
	};

	
}
