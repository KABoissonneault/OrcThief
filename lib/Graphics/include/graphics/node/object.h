#pragma once

#include "graphics/node/object.fwd.h"

#include "math/vector3.h"

namespace ot::graphics::node
{
	namespace detail
	{
		void init_object_impl(object&, void*) noexcept;
		void* get_scene_node_impl(object&) noexcept;
	}

	class object
	{
		std::byte storage[sizeof(void*)];

		friend void detail::init_object_impl(object&, void*) noexcept;
		friend void* detail::get_scene_node_impl(object&) noexcept;

		void destroy_node() noexcept;

	public:
		object() noexcept;
		object(object const&) = delete;
		object(object&&) noexcept;
		object& operator=(object const&) = delete;
		object& operator=(object&&) noexcept;
		~object();

		void set_position(math::vector3d position);
		void set_direction(math::vector3d direction);
		void rotate_around(math::vector3d axis, double rad);

		void attach_child(object& child);
	};

	
}
