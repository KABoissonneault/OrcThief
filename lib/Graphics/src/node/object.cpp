#include "object.h"

#include "Ogre/SceneNode.h"

namespace ot::graphics::node
{
	void init_object(object& n, Ogre::SceneNode* snode)
	{
		static_assert(sizeof(object::storage) == sizeof(Ogre::SceneNode*));
		memcpy(&n.storage, &snode, sizeof(Ogre::SceneNode*));
	}

	object::object(object&& other) noexcept
	{
		init_object(*this, &get_scene_node(other));
	}

	object& object::operator=(object&& other) noexcept
	{
		init_object(*this, &get_scene_node(other));
		return *this;
	}

	object::~object()
	{

	}

	Ogre::SceneNode& get_scene_node(object& n)
	{
		Ogre::SceneNode* snode;
		memcpy(&snode, &n.storage, sizeof(Ogre::SceneNode*));
		return *snode;
	}

	void set_position(object& n, math::vector3d p)
	{
		get_scene_node(n).setPosition(static_cast<Ogre::Real>(p.x), static_cast<Ogre::Real>(p.y), static_cast<Ogre::Real>(p.z));
	}

	void set_direction(object& n, math::vector3d direction)
	{
		get_scene_node(n).setDirection(static_cast<Ogre::Real>(direction.x), static_cast<Ogre::Real>(direction.y), static_cast<Ogre::Real>(direction.z));
	}

	void rotate_around(object& n, math::vector3d axis, double rad)
	{
		get_scene_node(n).rotate(
			Ogre::Vector3(static_cast<Ogre::Real>(axis.x), static_cast<Ogre::Real>(axis.y), static_cast<Ogre::Real>(axis.z)),
			Ogre::Radian(static_cast<Ogre::Real>(rad))
		);
	}
}