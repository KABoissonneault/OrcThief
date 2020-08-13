#include "object.h"

#include "Ogre/SceneNode.h"
#include "Ogre/SceneManager.h"

#include "ogre_conversion.h"

namespace ot::graphics::node
{
	namespace detail
	{
		void init_object_impl(object& n, void* snode) noexcept
		{
			n.pimpl = snode;
		}

		void* get_scene_node_impl(object const& n) noexcept
		{
			return n.pimpl;
		}
	}

	void init_object(object& n, Ogre::SceneNode* snode) noexcept
	{
		detail::init_object_impl(n, snode);
	}

	Ogre::SceneNode& get_scene_node(object& n) noexcept
	{
		return *static_cast<Ogre::SceneNode*>(detail::get_scene_node_impl(n));
	}

	Ogre::SceneNode const& get_scene_node(object const& n) noexcept
	{
		return *static_cast<Ogre::SceneNode const*>(detail::get_scene_node_impl(n));
	}

	object::object() noexcept
	{
		init_object(*this, nullptr);
	}

	object::object(object&& other) noexcept
	{
		init_object(*this, static_cast<Ogre::SceneNode*>(other.pimpl));
		init_object(other, nullptr);
	}

	object& object::operator=(object&& other) noexcept
	{
		if (this != &other)
		{
			destroy_node();
			init_object(*this, static_cast<Ogre::SceneNode*>(other.pimpl));
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
		auto const snode = static_cast<Ogre::SceneNode*>(pimpl);

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

	node_id object::get_node_id() const noexcept
	{
		Ogre::SceneNode const& scene_node = get_scene_node(*this);
		return node_id(scene_node.getId());
	}

	bool object::contains(object_id id) const noexcept
	{
		Ogre::SceneNode const& scene_node = get_scene_node(*this);
		Ogre::SceneNode::ConstObjectIterator attached_objects = scene_node.getAttachedObjectIterator();
		for (Ogre::MovableObject const* const sub_object : attached_objects)
		{
			if (sub_object->getId() == static_cast<Ogre::IdType>(id))
				return true;
		}

		return false;
	}

	void object::set_position(math::point3d p)
	{
		get_scene_node(*this).setPosition(to_ogre_vector(p));
	}

	math::point3d object::get_position() const noexcept
	{
		return to_math_point(get_scene_node(*this).getPosition());
	}

	math::quaternion object::get_rotation() const noexcept
	{
		return to_math_quaternion(get_scene_node(*this).getOrientation());
	}

	double object::get_scale() const noexcept
	{
		return get_scene_node(*this).getScale()[0]; // we only do uniform scaling here
	}

	void object::set_direction(math::vector3d direction)
	{
		get_scene_node(*this).setDirection(to_ogre_vector(direction));
	}

	void object::rotate_around(math::vector3d axis, double rad)
	{
		get_scene_node(*this).rotate(
			Ogre::Vector3(static_cast<Ogre::Real>(axis.x), static_cast<Ogre::Real>(axis.y), static_cast<Ogre::Real>(axis.z)),
			Ogre::Radian(static_cast<Ogre::Real>(rad))
		);
	}

	void object::attach_child(object& child)
	{
		get_scene_node(*this).addChild(&get_scene_node(child));
	}
}
