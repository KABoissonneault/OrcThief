#include "object.h"

#include "Ogre/SceneNode.h"
#include "Ogre/SceneManager.h"

#include "ogre_conversion.h"

#include <utility>
#include <memory>

namespace ot::egfx::node
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

		template<typename Derived>
		node_id object_const_impl<Derived>::get_node_id() const noexcept
		{
			Ogre::SceneNode const& scene_node = get_scene_node(static_cast<derived const&>(*this));
			return node_id(scene_node.getId());
		}

		template<typename Derived>
		bool object_const_impl<Derived>::contains(object_id id) const noexcept
		{
			Ogre::SceneNode const& scene_node = get_scene_node(static_cast<derived const&>(*this));
			Ogre::SceneNode::ConstObjectIterator attached_objects = scene_node.getAttachedObjectIterator();
			for (Ogre::MovableObject const* const sub_object : attached_objects)
			{
				if (sub_object->getId() == static_cast<Ogre::IdType>(id))
					return true;
			}

			return false;
		}

		template<typename Derived>
		math::point3f object_const_impl<Derived>::get_position() const noexcept
		{
			return to_math_point(get_scene_node(static_cast<derived const&>(*this)).getPosition());
		}

		template<typename Derived>
		math::quaternion object_const_impl<Derived>::get_rotation() const noexcept
		{
			return to_math_quaternion(get_scene_node(static_cast<derived const&>(*this)).getOrientation());
		}

		template<typename Derived>
		math::scales object_const_impl<Derived>::get_scale() const noexcept
		{
			return to_math_scales(get_scene_node(static_cast<derived const&>(*this)).getScale());
		}

		template<typename Derived>
		void* object_const_impl<Derived>::get_user_ptr() const noexcept
		{
			Ogre::SceneNode const& scene_node = get_scene_node(static_cast<derived const&>(*this));
			Ogre::UserObjectBindings const& user_object_bindings = scene_node.getUserObjectBindings();
			Ogre::Any const any = user_object_bindings.getUserAny();
			return any.has_value() ? any.get<void*>() : nullptr;
		}

		template<typename Derived>
		size_t object_const_impl<Derived>::get_child_count() const noexcept
		{
			return get_scene_node(static_cast<derived const&>(*this)).numChildren();
		}

		template class object_const_impl<object_cref>;
		template class object_const_impl<object_ref>;
		template class object_const_impl<object>;
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

	object_iterator::object_iterator(void* node) noexcept
		: scene_node(node)
	{

	}

	object_iterator& object_iterator::operator++()
	{
		scene_node = static_cast<Ogre::Node**>(scene_node) + 1;
		return *this;
	}

	object_iterator object_iterator::operator++(int)
	{
		object_iterator old = *this;
		++(*this);
		return old;
	}

	auto object_iterator::operator*() const -> element_type
	{
		return make_object_ref(*static_cast<Ogre::SceneNode*>(*static_cast<Ogre::Node**>(scene_node)));
	}

	auto object_iterator::operator->() const -> pointer
	{
		return pointer{ *(*this) };
	}

	bool object_iterator::operator==(object_iterator const& rhs) const noexcept
	{
		return scene_node == rhs.scene_node;
	}

	object_range::object_range(object_iterator it, object_iterator sent) noexcept
		: it(it)
		, sent(sent)
	{

	}

	object_const_iterator::object_const_iterator(void const* node) noexcept
		: scene_node(node)
	{

	}

	object_const_iterator::object_const_iterator(object_iterator it) noexcept
		: scene_node(it.scene_node)
	{

	}

	object_const_iterator& object_const_iterator::operator++()
	{
		scene_node = static_cast<Ogre::Node const* const*>(scene_node) + 1;
		return *this;
	}

	object_const_iterator object_const_iterator::operator++(int)
	{
		object_const_iterator old = *this;
		++(*this);
		return old;
	}

	auto object_const_iterator::operator*() const -> element_type
	{
		return make_object_cref(*static_cast<Ogre::SceneNode const*>(*static_cast<Ogre::Node const* const*>(scene_node)));
	}

	auto object_const_iterator::operator->() const -> pointer
	{
		return pointer{ *(*this) };
	}

	bool object_const_iterator::operator==(object_const_iterator const& rhs) const noexcept
	{
		return scene_node == rhs.scene_node;
	}

	object_const_range::object_const_range(object_const_iterator it, object_const_iterator sent) noexcept
		: it(it)
		, sent(sent)
	{

	}

	object_const_range::object_const_range(object_range range) noexcept
		: it(range.begin())
		, sent(range.end())
	{

	}

	std::optional<object_cref> object_cref::get_parent() const noexcept
	{
		Ogre::SceneNode const* parent = get_scene_node(*this).getParentSceneNode();
		if (parent == nullptr)
			return std::nullopt;
		return make_object_cref(*parent);
	}

	std::optional<object_cref> object_cref::get_child(size_t i) const
	{
		// All Ogre::Node seem to be SceneNode
		Ogre::SceneNode const* child = static_cast<Ogre::SceneNode const*>(get_scene_node(*this).getChild(i));
		if (child == nullptr)
			return std::nullopt;
		return make_object_cref(*child);
	}

	object_const_range object_cref::get_children() const noexcept
	{
		Ogre::Node::ConstNodeVecIterator it = get_scene_node(*this).getChildIterator();
		return { std::to_address(it.begin()), std::to_address(it.end()) };
	}

	std::optional<object_ref> object_ref::get_parent() const noexcept
	{
		Ogre::SceneNode* const parent = get_scene_node(*this).getParentSceneNode();
		if (parent == nullptr)
			return std::nullopt;
		return make_object_ref(*parent);
	}

	std::optional<object_ref> object_ref::get_child(size_t i) const
	{
		Ogre::SceneNode* const child = static_cast<Ogre::SceneNode*>(get_scene_node(*this).getChild(i));
		if (child == nullptr)
			return std::nullopt;
		return make_object_ref(*child);
	}

	object_range object_ref::get_children() const noexcept
	{
		Ogre::Node::NodeVecIterator it = get_scene_node(*this).getChildIterator();
		return { std::to_address(it.begin()), std::to_address(it.end()) };
	}

	void object_ref::set_user_ptr(void* user_ptr) const
	{
		get_scene_node(*this).getUserObjectBindings().setUserAny(Ogre::Any(user_ptr));
	}

	object::object() noexcept
		: pimpl(nullptr)
	{

	}

	object::object(object&& other) noexcept
		: pimpl(std::exchange(other.pimpl, nullptr))
	{

	}

	object& object::operator=(object&& other) noexcept
	{
		if (this != &other)
		{
			destroy_node();
			pimpl = std::exchange(other.pimpl, nullptr);
		}
		return *this;
	}

	object::~object()
	{
		destroy_node();
	}

	void object::destroy_node() noexcept
	{
		if (pimpl == nullptr)
			return;

		Ogre::SceneNode& snode = get_scene_node(*this);
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

	void object::set_impl(object_ref r) noexcept
	{
		pimpl = detail::get_object_impl(r);
	}

	void object_ref::set_position(math::point3f p) const noexcept
	{
		get_scene_node(*this).setPosition(to_ogre_vector(p));
	}

	void object::set_position(math::point3f p) noexcept
	{
		static_cast<object_ref>(*this).set_position(p);
	}

	void object_ref::set_rotation(math::quaternion rot) const noexcept
	{
		get_scene_node(*this).setOrientation(to_ogre_quaternion(rot));
	}

	void object::set_rotation(math::quaternion rot) noexcept
	{
		static_cast<object_ref>(*this).set_rotation(rot);
	}

	void object_ref::set_direction(math::vector3f direction) const noexcept
	{
		get_scene_node(*this).setDirection(to_ogre_vector(direction));
	}

	void object::set_direction(math::vector3f direction) noexcept
	{
		static_cast<object_ref>(*this).set_direction(direction);
	}

	void object_ref::rotate_around(math::vector3f axis, float rad) const noexcept
	{
		get_scene_node(*this).rotate(
			Ogre::Vector3(static_cast<Ogre::Real>(axis.x), static_cast<Ogre::Real>(axis.y), static_cast<Ogre::Real>(axis.z)),
			Ogre::Radian(static_cast<Ogre::Real>(rad))
		);
	}

	void object::rotate_around(math::vector3f axis, float rad) noexcept
	{
		static_cast<object_ref>(*this).rotate_around(axis, rad);
	}

	void object_ref::set_scale(float s) const noexcept
	{
		get_scene_node(*this).setScale(s, s, s);
	}

	void object::set_scale(float s) noexcept
	{
		static_cast<object_ref>(*this).set_scale(s);
	}

	void object_ref::set_scale(math::scales s) const noexcept
	{
		get_scene_node(*this).setScale(s.x, s.y, s.z);
	}

	void object::set_scale(math::scales s) noexcept
	{
		static_cast<object_ref>(*this).set_scale(s);
	}

	void object_ref::attach_child(object_ref child) const noexcept
	{
		get_scene_node(*this).addChild(&get_scene_node(child));
	}

	void object::attach_child(object_ref child) noexcept
	{
		static_cast<object_ref>(*this).attach_child(child);
	}

	std::optional<object_cref> object::get_parent() const noexcept
	{
		return static_cast<object_cref>(*this).get_parent();
	}

	std::optional<object_ref> object::get_parent() noexcept
	{
		return static_cast<object_ref>(*this).get_parent();
	}

	std::optional<object_cref> object::get_child(size_t i) const
	{
		return static_cast<object_cref>(*this).get_child(i);
	}

	std::optional<object_ref> object::get_child(size_t i)
	{
		return static_cast<object_ref>(*this).get_child(i);
	}

	object_const_range object::get_children() const noexcept
	{
		return static_cast<object_cref>(*this).get_children();
	}

	object_range object::get_children() noexcept
	{
		return static_cast<object_ref>(*this).get_children();
	}

	void object::set_user_ptr(void* user_ptr)
	{
		get_scene_node(*this).getUserObjectBindings().setUserAny(Ogre::Any(user_ptr));
	}
}
