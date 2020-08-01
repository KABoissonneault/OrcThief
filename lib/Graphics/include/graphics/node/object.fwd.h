#pragma once

#include <cstddef>

namespace Ogre
{
	class SceneNode;
}

namespace ot::graphics::node
{
	class object
	{
		std::byte storage[sizeof(void*)];

		friend void init_object(object&, Ogre::SceneNode*) noexcept;
		friend Ogre::SceneNode& get_scene_node(object&) noexcept;

		void destroy_node() noexcept;

	public:
		object() noexcept;
		object(object const&) = delete;
		object(object&&) noexcept;
		object& operator=(object const&) = delete;
		object& operator=(object&&) noexcept;
		~object();
	};
}