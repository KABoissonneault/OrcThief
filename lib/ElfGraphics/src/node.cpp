#include "node.h"

#include "object/object.h"

#include "Ogre/SceneNode.h"
#include "Ogre/SceneManager.h"

#include "ogre_conversion.h"

#include <utility>
#include <memory>

namespace ot::egfx
{
	namespace detail
	{
		node_cref make_node_cref(void const* pimpl) noexcept
		{
			node_cref o;
			o.pimpl = pimpl;
			return o;
		}

		node_ref make_node_ref(void* pimpl) noexcept
		{
			node_ref o;
			o.pimpl = pimpl;
			return o;
		}

		void const* get_node_impl(node_cref r) noexcept
		{
			return r.pimpl;
		}

		void* get_node_impl(node_ref r) noexcept
		{
			return r.pimpl;
		}

		template<typename Derived>
		node_id node_const_impl<Derived>::get_node_id() const noexcept
		{
			Ogre::SceneNode const& scene_node = get_scene_node(static_cast<derived const&>(*this));
			return node_id(scene_node.getId());
		}

		template<typename Derived>
		bool node_const_impl<Derived>::contains(object_id id) const noexcept
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
		math::point3f node_const_impl<Derived>::get_position() const noexcept
		{
			return to_math_point(get_scene_node(static_cast<derived const&>(*this)).getPosition());
		}

		template<typename Derived>
		math::quaternion node_const_impl<Derived>::get_rotation() const noexcept
		{
			return to_math_quaternion(get_scene_node(static_cast<derived const&>(*this)).getOrientation());
		}

		template<typename Derived>
		math::scales node_const_impl<Derived>::get_scale() const noexcept
		{
			return to_math_scales(get_scene_node(static_cast<derived const&>(*this)).getScale());
		}

		template<typename Derived>
		void* node_const_impl<Derived>::get_user_ptr() const noexcept
		{
			Ogre::SceneNode const& scene_node = get_scene_node(static_cast<derived const&>(*this));
			Ogre::UserObjectBindings const& user_object_bindings = scene_node.getUserObjectBindings();
			Ogre::Any const any = user_object_bindings.getUserAny();
			return any.has_value() ? any.get<void*>() : nullptr;
		}

		template<typename Derived>
		size_t node_const_impl<Derived>::get_child_count() const noexcept
		{
			return get_scene_node(static_cast<derived const&>(*this)).numChildren();
		}

		template<typename Derived>
		size_t node_const_impl<Derived>::get_object_count() const noexcept
		{
			return get_scene_node(static_cast<derived const&>(*this)).numAttachedObjects();
		}

		template class node_const_impl<node_cref>;
		template class node_const_impl<node_ref>;
		template class node_const_impl<node>;
	}

	node_cref make_node_cref(Ogre::SceneNode const& node) noexcept
	{
		return detail::make_node_cref(&node);
	}

	node_ref make_node_ref(Ogre::SceneNode& node) noexcept
	{
		return detail::make_node_ref(&node);
	}

	Ogre::SceneNode const& get_scene_node(node_cref r) noexcept
	{
		return *static_cast<Ogre::SceneNode const*>(detail::get_node_impl(r));
	}

	Ogre::SceneNode& get_scene_node(node_ref r) noexcept
	{
		return *static_cast<Ogre::SceneNode*>(detail::get_node_impl(r));
	}

	node_iterator::node_iterator(void* node) noexcept
		: scene_node(node)
	{

	}

	node_iterator& node_iterator::operator++()
	{
		scene_node = static_cast<Ogre::Node**>(scene_node) + 1;
		return *this;
	}

	node_iterator node_iterator::operator++(int)
	{
		node_iterator old = *this;
		++(*this);
		return old;
	}

	node_iterator& node_iterator::operator+=(difference_type n)
	{
		scene_node = static_cast<Ogre::Node**>(scene_node) + n;
		return *this;
	}

	node_iterator operator+(node_iterator it, node_iterator::difference_type n)
	{
		it += n;
		return it;
	}

	node_iterator operator+(node_iterator::difference_type n, node_iterator it)
	{
		it += n;
		return it;
	}

	node_iterator& node_iterator::operator--()
	{
		scene_node = static_cast<Ogre::Node**>(scene_node) - 1;
		return *this;
	}

	node_iterator node_iterator::operator--(int)
	{
		node_iterator old = *this;
		--(*this);
		return old;
	}

	node_iterator& node_iterator::operator-=(difference_type n)
	{
		scene_node = static_cast<Ogre::Node**>(scene_node) - n;
		return *this;
	}

	node_iterator node_iterator::operator-(difference_type n) const
	{
		node_iterator it = *this;
		it -= n;
		return it;
	}

	auto node_iterator::operator-(node_iterator const& rhs) const noexcept -> difference_type
	{
		return static_cast<Ogre::Node**>(scene_node) - static_cast<Ogre::Node**>(rhs.scene_node);
	}

	auto node_iterator::operator*() const -> reference
	{
		return make_node_ref(*static_cast<Ogre::SceneNode*>(*static_cast<Ogre::Node**>(scene_node)));
	}

	auto node_iterator::operator->() const -> pointer
	{
		return pointer{ *(*this) };
	}

	auto node_iterator::operator[](difference_type n) const -> reference
	{
		return make_node_ref(*static_cast<Ogre::SceneNode*>(static_cast<Ogre::Node**>(scene_node)[n]));
	}

	node_range::node_range(node_iterator it, node_iterator sent) noexcept
		: it(it)
		, sent(sent)
	{

	}

	node_const_iterator::node_const_iterator(void const* node) noexcept
		: scene_node(node)
	{

	}

	node_const_iterator::node_const_iterator(node_iterator it) noexcept
		: scene_node(it.scene_node)
	{

	}

	node_const_iterator& node_const_iterator::operator++()
	{
		scene_node = static_cast<Ogre::Node const* const*>(scene_node) + 1;
		return *this;
	}

	node_const_iterator node_const_iterator::operator++(int)
	{
		node_const_iterator old = *this;
		++(*this);
		return old;
	}

	node_const_iterator& node_const_iterator::operator+=(difference_type n)
	{
		scene_node = static_cast<Ogre::Node const* const*>(scene_node) + n;
		return *this;
	}

	node_const_iterator operator+(node_const_iterator it, node_const_iterator::difference_type n)
	{
		it += n;
		return it;
	}

	node_const_iterator operator+(node_const_iterator::difference_type n, node_const_iterator it)
	{
		it += n;
		return it;
	}


	node_const_iterator& node_const_iterator::operator--()
	{
		scene_node = static_cast<Ogre::Node const* const*>(scene_node) - 1;
		return *this;
	}

	node_const_iterator node_const_iterator::operator--(int)
	{
		node_const_iterator old = *this;
		--(*this);
		return old;
	}

	node_const_iterator& node_const_iterator::operator-=(difference_type n)
	{
		scene_node = static_cast<Ogre::Node const* const*>(scene_node) - n;
		return *this;
	}

	node_const_iterator node_const_iterator::operator-(difference_type n) const
	{
		node_const_iterator it = *this;
		it -= n;
		return it;
	}

	auto node_const_iterator::operator-(node_const_iterator const& rhs) const noexcept -> difference_type
	{
		return static_cast<Ogre::Node const* const*>(scene_node) - static_cast<Ogre::Node const* const*>(rhs.scene_node);
	}

	auto node_const_iterator::operator*() const -> element_type
	{
		return make_node_cref(*static_cast<Ogre::SceneNode const*>(*static_cast<Ogre::Node const* const*>(scene_node)));
	}

	auto node_const_iterator::operator->() const -> pointer
	{
		return pointer{ *(*this) };
	}

	auto node_const_iterator::operator[](difference_type n) const -> reference
	{
		return make_node_cref(*static_cast<Ogre::SceneNode const*>(static_cast<Ogre::Node const* const*>(scene_node)[n]));
	}

	node_const_range::node_const_range(node_const_iterator it, node_const_iterator sent) noexcept
		: it(it)
		, sent(sent)
	{

	}

	node_const_range::node_const_range(node_range range) noexcept
		: it(range.begin())
		, sent(range.end())
	{

	}

	std::optional<node_cref> node_cref::get_parent() const noexcept
	{
		Ogre::SceneNode const* parent = get_scene_node(*this).getParentSceneNode();
		if (parent == nullptr)
			return std::nullopt;
		return make_node_cref(*parent);
	}

	node_cref node_cref::get_child(size_t i) const
	{
		// All Ogre::Node seem to be SceneNode
		Ogre::SceneNode const* child = static_cast<Ogre::SceneNode const*>(get_scene_node(*this).getChild(i));
		return make_node_cref(*child);
	}

	object_cref node_cref::get_object(size_t i) const
	{
		// Can only get by index on non-const
		auto& scene_node = const_cast<Ogre::SceneNode&>(get_scene_node(*this));
		
		Ogre::MovableObject const& object = *scene_node.getAttachedObject(i);
		return make_object_cref(object);
	}

	node_const_range node_cref::get_children() const noexcept
	{
		Ogre::Node::ConstNodeVecIterator it = get_scene_node(*this).getChildIterator();
		return { std::to_address(it.begin()), std::to_address(it.end()) };
	}

	object_const_range node_cref::get_objects() const noexcept
	{
		Ogre::SceneNode::ConstObjectIterator it = get_scene_node(*this).getAttachedObjectIterator();
		return { std::to_address(it.begin()), std::to_address(it.end()) };
	}

	std::optional<node_ref> node_ref::get_parent() const noexcept
	{
		Ogre::SceneNode* const parent = get_scene_node(*this).getParentSceneNode();
		if (parent == nullptr)
			return std::nullopt;
		return make_node_ref(*parent);
	}

	node_ref node_ref::get_child(size_t i) const
	{
		Ogre::SceneNode* const child = static_cast<Ogre::SceneNode*>(get_scene_node(*this).getChild(i));
		return make_node_ref(*child);
	}

	object_ref node_ref::get_object(size_t i) const
	{
		// Can only get by index on non-const
		auto& scene_node = get_scene_node(*this);

		Ogre::MovableObject& object = *scene_node.getAttachedObject(i);
		return make_object_ref(object);
	}

	node_range node_ref::get_children() const noexcept
	{
		Ogre::Node::NodeVecIterator it = get_scene_node(*this).getChildIterator();
		return { std::to_address(it.begin()), std::to_address(it.end()) };
	}

	object_range node_ref::get_objects() const noexcept
	{
		Ogre::SceneNode::ObjectIterator it = get_scene_node(*this).getAttachedObjectIterator();
		return { std::to_address(it.begin()), std::to_address(it.end()) };
	}

	void node_ref::set_user_ptr(void* user_ptr) const
	{
		get_scene_node(*this).getUserObjectBindings().setUserAny(Ogre::Any(user_ptr));
	}

	node::node() noexcept
		: pimpl(nullptr)
	{

	}

	node::node(node&& other) noexcept
		: pimpl(std::exchange(other.pimpl, nullptr))
	{

	}

	node& node::operator=(node&& other) noexcept
	{
		if (this != &other)
		{
			destroy_node();
			pimpl = std::exchange(other.pimpl, nullptr);
		}
		return *this;
	}

	node::~node()
	{
		destroy_node();
	}

	void node::destroy_node() noexcept
	{
		if (pimpl == nullptr)
			return;

		Ogre::SceneNode& snode = get_scene_node(*this);
		Ogre::SceneManager* const creator_scene = snode.getCreator();
		if (creator_scene == nullptr)
			return;

		// Destroy all attached nodes
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

	void node::set_impl(node_ref r) noexcept
	{
		pimpl = detail::get_node_impl(r);
	}

	void node_ref::set_position(math::point3f p) const noexcept
	{
		get_scene_node(*this).setPosition(to_ogre_vector(p));
	}

	void node::set_position(math::point3f p) noexcept
	{
		static_cast<node_ref>(*this).set_position(p);
	}

	void node_ref::set_rotation(math::quaternion rot) const noexcept
	{
		get_scene_node(*this).setOrientation(to_ogre_quaternion(rot));
	}

	void node::set_rotation(math::quaternion rot) noexcept
	{
		static_cast<node_ref>(*this).set_rotation(rot);
	}

	void node_ref::set_direction(math::vector3f direction) const noexcept
	{
		get_scene_node(*this).setDirection(to_ogre_vector(direction));
	}

	void node::set_direction(math::vector3f direction) noexcept
	{
		static_cast<node_ref>(*this).set_direction(direction);
	}

	void node_ref::rotate_around(math::vector3f axis, float rad) const noexcept
	{
		get_scene_node(*this).rotate(
			Ogre::Vector3(static_cast<Ogre::Real>(axis.x), static_cast<Ogre::Real>(axis.y), static_cast<Ogre::Real>(axis.z)),
			Ogre::Radian(static_cast<Ogre::Real>(rad))
		);
	}

	void node::rotate_around(math::vector3f axis, float rad) noexcept
	{
		static_cast<node_ref>(*this).rotate_around(axis, rad);
	}

	void node_ref::set_scale(float s) const noexcept
	{
		get_scene_node(*this).setScale(s, s, s);
	}

	void node::set_scale(float s) noexcept
	{
		static_cast<node_ref>(*this).set_scale(s);
	}

	void node_ref::set_scale(math::scales s) const noexcept
	{
		get_scene_node(*this).setScale(s.x, s.y, s.z);
	}

	void node::set_scale(math::scales s) noexcept
	{
		static_cast<node_ref>(*this).set_scale(s);
	}

	void node_ref::attach_child(node_ref child) const noexcept
	{
		get_scene_node(*this).addChild(&get_scene_node(child));
	}

	void node::attach_child(node_ref child) noexcept
	{
		static_cast<node_ref>(*this).attach_child(child);
	}

	std::optional<node_cref> node::get_parent() const noexcept
	{
		return static_cast<node_cref>(*this).get_parent();
	}

	std::optional<node_ref> node::get_parent() noexcept
	{
		return static_cast<node_ref>(*this).get_parent();
	}

	std::optional<node_cref> node::get_child(size_t i) const
	{
		return static_cast<node_cref>(*this).get_child(i);
	}

	std::optional<node_ref> node::get_child(size_t i)
	{
		return static_cast<node_ref>(*this).get_child(i);
	}

	node_const_range node::get_children() const noexcept
	{
		return static_cast<node_cref>(*this).get_children();
	}

	node_range node::get_children() noexcept
	{
		return static_cast<node_ref>(*this).get_children();
	}

	object_cref node::get_object(size_t i) const
	{
		return static_cast<node_cref>(*this).get_object(i);
	}

	object_ref node::get_object(size_t i)
	{
		return static_cast<node_ref>(*this).get_object(i);
	}

	object_const_range node::get_objects() const noexcept
	{
		return static_cast<node_cref>(*this).get_objects();
	}

	object_range node::get_objects() noexcept
	{
		return static_cast<node_ref>(*this).get_objects();
	}

	void node::set_user_ptr(void* user_ptr)
	{
		get_scene_node(*this).getUserObjectBindings().setUserAny(Ogre::Any(user_ptr));
	}

	node create_child_node(node_ref parent)
	{
		Ogre::SceneNode* const new_node = get_scene_node(parent).createChildSceneNode();
		node n;
		n.set_impl(make_node_ref(*new_node));
		return n;
	}
}
