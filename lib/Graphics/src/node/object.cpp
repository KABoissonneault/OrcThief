#include "object.h"

#include "Ogre/SceneNode.h"
#include "Ogre/SceneManager.h"

#include "ogre_conversion.h"

#include <utility>

namespace ot::graphics::node
{
	namespace detail
	{
		object_cref make_object_cref(void const* pimpl) noexcept
		{
			object_cref o;
			o.pimpl = pimpl;
			return o;
		}
		
		object_ref make_object_ref(void* pimpl) noexcept
		{
			object_ref o;
			o.pimpl = pimpl;
			return o;
		}
		
		void const* get_object_impl(object_cref r) noexcept
		{
			return r.pimpl;
		}

		void* get_object_impl(object_ref r) noexcept
		{
			return r.pimpl;
		}
	}

	object_cref make_null_cref() noexcept
	{
		return detail::make_object_cref(nullptr);
	}

	object_ref make_null_ref() noexcept
	{
		return detail::make_object_ref(nullptr);
	}

	bool is_null(object_cref r) noexcept
	{
		return detail::get_object_impl(r) == nullptr;
	}

	bool is_null(object_ref r) noexcept
	{
		return detail::get_object_impl(r) == nullptr;
	}

	object_cref make_object_cref(Ogre::SceneNode const& node) noexcept
	{
		return detail::make_object_cref(&node);
	}

	object_ref make_object_ref(Ogre::SceneNode& node) noexcept
	{
		return detail::make_object_ref(&node);
	}

	Ogre::SceneNode const& get_scene_node(object_cref r) noexcept
	{
		return *static_cast<Ogre::SceneNode const*>(detail::get_object_impl(r));
	}

	Ogre::SceneNode& get_scene_node(object_ref r) noexcept
	{
		return *static_cast<Ogre::SceneNode*>(detail::get_object_impl(r));
	}

	object::object() noexcept
		: ref(make_null_ref())
	{
		
	}

	object::object(object&& other) noexcept
		: ref(std::exchange(other.ref, make_null_ref()))
	{

	}

	object& object::operator=(object&& other) noexcept
	{
		if (this != &other)
		{
			destroy_node();
			ref = std::exchange(other.ref, make_null_ref());
		}
		return *this;
	}

	object::~object()
	{
		destroy_node();
	}

	void object::destroy_node() noexcept
	{
		if (is_null(ref))
			return;

		Ogre::SceneNode& snode = get_scene_node(ref);
		Ogre::SceneManager* const creator_scene = snode.getCreator();
		if (creator_scene == nullptr)
			return;
		
		// Destroy all attached objects
		std::vector<Ogre::MovableObject*> attached_objects;
		for (Ogre::MovableObject* attached_object : snode.getAttachedObjectIterator())
		{
			attached_objects.push_back(attached_object);
		}

		for (Ogre::MovableObject* attached_object : attached_objects)
		{
			creator_scene->destroyMovableObject(attached_object);
		}

		// Destroy node
		creator_scene->destroySceneNode(&snode);
	}

	node_id object_cref::get_node_id() const noexcept
	{
		Ogre::SceneNode const& scene_node = get_scene_node(*this);
		return node_id(scene_node.getId());
	}

	node_id object_ref::get_node_id() const noexcept
	{
		return static_cast<object_cref>(*this).get_node_id();
	}

	node_id object::get_node_id() const noexcept
	{
		return static_cast<object_cref>(*this).get_node_id();
	}

	bool object_cref::contains(object_id id) const noexcept
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

	bool object_ref::contains(object_id id) const noexcept
	{
		return static_cast<object_cref>(*this).contains(id);
	}

	bool object::contains(object_id id) const noexcept
	{
		return static_cast<object_cref>(*this).contains(id);
	}

	void object_ref::set_position(math::point3d p) const noexcept
	{
		get_scene_node(*this).setPosition(to_ogre_vector(p));
	}

	void object::set_position(math::point3d p) noexcept
	{
		static_cast<object_ref>(*this).set_position(p);
	}

	math::point3d object_cref::get_position() const noexcept
	{
		return to_math_point(get_scene_node(*this).getPosition());
	}

	math::point3d object_ref::get_position() const noexcept
	{
		return static_cast<object_cref>(*this).get_position();
	}

	math::point3d object::get_position() const noexcept
	{
		return static_cast<object_cref>(*this).get_position();
	}

	math::quaternion object_cref::get_rotation() const noexcept
	{
		return to_math_quaternion(get_scene_node(*this).getOrientation());
	}

	math::quaternion object_ref::get_rotation() const noexcept
	{
		return static_cast<object_cref>(*this).get_rotation();
	}

	math::quaternion object::get_rotation() const noexcept
	{
		return static_cast<object_cref>(*this).get_rotation();
	}

	double object_cref::get_scale() const noexcept
	{
		return get_scene_node(*this).getScale()[0]; // we only do uniform scaling here
	}

	double object_ref::get_scale() const noexcept
	{
		return static_cast<object_cref>(*this).get_scale();
	}

	double object::get_scale() const noexcept
	{
		return static_cast<object_cref>(*this).get_scale();
	}

	void object_ref::set_direction(math::vector3d direction) const noexcept
	{
		get_scene_node(*this).setDirection(to_ogre_vector(direction));
	}

	void object::set_direction(math::vector3d direction) noexcept
	{
		static_cast<object_ref>(*this).set_direction(direction);
	}

	void object_ref::rotate_around(math::vector3d axis, double rad) const noexcept
	{
		get_scene_node(*this).rotate(
			Ogre::Vector3(static_cast<Ogre::Real>(axis.x), static_cast<Ogre::Real>(axis.y), static_cast<Ogre::Real>(axis.z)),
			Ogre::Radian(static_cast<Ogre::Real>(rad))
		);
	}

	void object::rotate_around(math::vector3d axis, double rad) noexcept
	{
		static_cast<object_ref>(*this).rotate_around(axis, rad);
	}

	void object_ref::attach_child(object_ref child) const noexcept
	{
		get_scene_node(*this).addChild(&get_scene_node(child));
	}

	void object::attach_child(object_ref child) noexcept
	{
		static_cast<object_ref>(*this).attach_child(child);
	}
}
