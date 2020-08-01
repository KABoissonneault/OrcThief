#include "object.h"

#include "Ogre/SceneNode.h"
#include "Ogre/SceneManager.h"

namespace ot::graphics::node
{
	void init_object(object& n, Ogre::SceneNode* snode) noexcept
	{
		static_assert(sizeof(object::storage) == sizeof(Ogre::SceneNode*));
		memcpy(&n.storage, &snode, sizeof(Ogre::SceneNode*));
	}

	object::object() noexcept
	{
		init_object(*this, nullptr);
	}

	object::object(object&& other) noexcept
	{
		init_object(*this, &get_scene_node(other));
		init_object(other, nullptr);
	}

	object& object::operator=(object&& other) noexcept
	{
		if (this != &other)
		{
			destroy_node();
			init_object(*this, &get_scene_node(other));
			init_object(other, nullptr);
		}
		return *this;
	}

	object::~object()
	{
		destroy_node();
	}

	void object::destroy_node() noexcept
	{
		Ogre::SceneNode* snode;
		memcpy(&snode, &storage, sizeof(Ogre::SceneNode*));

		if (snode != nullptr && snode->getParent() != nullptr) // don't destroy root node
		{
			Ogre::SceneManager* const creator_scene = snode->getCreator();
			if (creator_scene != nullptr)
			{
				std::vector<Ogre::MovableObject*> attached_objects;
				for (Ogre::MovableObject* attached_object : snode->getAttachedObjectIterator())
				{
					attached_objects.push_back(attached_object);
				}

				for (Ogre::MovableObject* attached_object : attached_objects)
				{
					creator_scene->destroyMovableObject(attached_object);
				}

				creator_scene->destroySceneNode(snode);
			}
		}
	}

	Ogre::SceneNode& get_scene_node(object& n) noexcept
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

	void attach_child(object& n, object& child)
	{
		get_scene_node(n).addChild(&get_scene_node(child));
	}
}
