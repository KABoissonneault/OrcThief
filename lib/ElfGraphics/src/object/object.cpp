#include "object.h"

#include "node.h"

#include "core/directive.h"

#include "Ogre/MovableObject.h"
#include "Ogre/Item.h"
#include "Ogre/Camera.h"
#include "Ogre/Light.h"

namespace ot::egfx
{
	namespace detail
	{
		void init_object_cref(object_cref& ref, void const* pimpl) noexcept
		{
			ref.pimpl = pimpl;
		}

		void init_object_ref(object_ref& ref, void* pimpl) noexcept
		{
			ref.pimpl = pimpl;
		}

		object_cref make_object_cref(void const* pimpl) noexcept
		{
			object_cref o;
			init_object_cref(o, pimpl);
			return o;
		}

		object_ref make_object_ref(void* pimpl) noexcept
		{
			object_ref o;
			init_object_ref(o, pimpl);
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
		object_id object_const_impl<Derived>::get_object_id() const noexcept
		{
			Ogre::MovableObject const& object = get_object(static_cast<derived const&>(*this));
			return object_id(object.getId());
		}

		template<typename Derived>
		object_type object_const_impl<Derived>::get_object_type() const noexcept
		{
			Ogre::MovableObject const& object = get_object(static_cast<derived const&>(*this));
			return egfx::get_object_type(object);
		}

		template<typename Derived>
		bool object_const_impl<Derived>::is_a(object_type t) const noexcept
		{
			Ogre::MovableObject const& object = get_object(static_cast<derived const&>(*this));
			return egfx::is_object_of_type(object, t);
		}

		template class object_const_impl<object_cref>;
		template class object_const_impl<object_ref>;
	}

	void init_object_cref(object_cref& ref, Ogre::MovableObject const& o) noexcept
	{
		detail::init_object_cref(ref, &o);
	}

	void init_object_ref(object_ref& ref, Ogre::MovableObject& o) noexcept
	{
		detail::init_object_ref(ref, &o);
	}

	object_cref make_object_cref(Ogre::MovableObject const& object) noexcept
	{
		return detail::make_object_cref(&object);
	}

	object_ref make_object_ref(Ogre::MovableObject& object) noexcept
	{
		return detail::make_object_ref(&object);
	}

	Ogre::MovableObject const& get_object(object_cref r) noexcept
	{
		return *static_cast<Ogre::MovableObject const*>(detail::get_object_impl(r));
	}

	Ogre::MovableObject& get_object(object_ref r) noexcept
	{
		return *static_cast<Ogre::MovableObject*>(detail::get_object_impl(r));
	}

	object_type get_object_type(Ogre::MovableObject const& obj)
	{
		std::string const& s = obj.getMovableType();
		if (s == Ogre::ItemFactory::FACTORY_TYPE_NAME)
		{
			return object_type::item;
		}
		else if (s == "Camera") // Ogre::Camera::msMovableType is protected
		{
			return object_type::camera;
		}
		else if (s == Ogre::LightFactory::FACTORY_TYPE_NAME)
		{
			return object_type::light;
		}
		else
		{
			assert(false && "Missing object type");
			OT_UNREACHABLE();
		}
	}

	bool is_object_of_type(Ogre::MovableObject const& obj, object_type t)
	{
		switch (t)
		{
		case object_type::item:
			return obj.getMovableType() == Ogre::ItemFactory::FACTORY_TYPE_NAME;
		case object_type::camera:
			return obj.getMovableType() == "Camera";
		case object_type::light:
			return obj.getMovableType() == Ogre::LightFactory::FACTORY_TYPE_NAME;
		}

		assert(false);
		OT_UNREACHABLE();
	}

	object_iterator::object_iterator(void* object_ptr_it) noexcept
		: object_ptr_it(object_ptr_it)
	{

	}

	object_iterator& object_iterator::operator++()
	{
		object_ptr_it = static_cast<Ogre::MovableObject**>(object_ptr_it) + 1;
		return *this;
	}

	object_iterator object_iterator::operator++(int)
	{
		object_iterator old = *this;
		++(*this);
		return old;
	}

	object_iterator& object_iterator::operator+=(difference_type n)
	{
		object_ptr_it = static_cast<Ogre::MovableObject**>(object_ptr_it) + n;
		return *this;
	}

	object_iterator operator+(object_iterator it, object_iterator::difference_type n)
	{
		it += n;
		return it;
	}

	object_iterator operator+(object_iterator::difference_type n, object_iterator it)
	{
		it += n;
		return it;
	}

	object_iterator& object_iterator::operator--()
	{
		object_ptr_it = static_cast<Ogre::MovableObject**>(object_ptr_it) - 1;
		return *this;
	}

	object_iterator object_iterator::operator--(int)
	{
		object_iterator old = *this;
		--(*this);
		return old;
	}

	object_iterator& object_iterator::operator-=(difference_type n)
	{
		object_ptr_it = static_cast<Ogre::MovableObject**>(object_ptr_it) - n;
		return *this;
	}

	object_iterator object_iterator::operator-(difference_type n) const
	{
		object_iterator it = *this;
		it -= n;
		return it;
	}

	auto object_iterator::operator-(object_iterator const& rhs) const noexcept -> difference_type
	{
		return static_cast<Ogre::MovableObject**>(object_ptr_it) - static_cast<Ogre::MovableObject**>(rhs.object_ptr_it);
	}

	auto object_iterator::operator*() const -> reference
	{
		return make_object_ref(**static_cast<Ogre::MovableObject**>(object_ptr_it));
	}

	auto object_iterator::operator->() const -> pointer
	{
		return pointer{ *(*this) };
	}

	auto object_iterator::operator[](difference_type n) const -> reference
	{
		return make_object_ref(*static_cast<Ogre::MovableObject**>(object_ptr_it)[n]);
	}

	object_range::object_range(object_iterator it, object_iterator sent) noexcept
		: it(it)
		, sent(sent)
	{

	}

	object_const_iterator::object_const_iterator(void const* object_ptr_it) noexcept
		: object_ptr_it(object_ptr_it)
	{

	}

	object_const_iterator::object_const_iterator(object_iterator it) noexcept
		: object_ptr_it(it.object_ptr_it)
	{

	}

	object_const_iterator& object_const_iterator::operator++()
	{
		object_ptr_it = static_cast<Ogre::MovableObject const* const*>(object_ptr_it) + 1;
		return *this;
	}

	object_const_iterator object_const_iterator::operator++(int)
	{
		object_const_iterator old = *this;
		++(*this);
		return old;
	}

	object_const_iterator& object_const_iterator::operator+=(difference_type n)
	{
		object_ptr_it = static_cast<Ogre::MovableObject const* const*>(object_ptr_it) + n;
		return *this;
	}

	object_const_iterator operator+(object_const_iterator it, object_const_iterator::difference_type n)
	{
		it += n;
		return it;
	}

	object_const_iterator operator+(object_const_iterator::difference_type n, object_const_iterator it)
	{
		it += n;
		return it;
	}


	object_const_iterator& object_const_iterator::operator--()
	{
		object_ptr_it = static_cast<Ogre::MovableObject const* const*>(object_ptr_it) - 1;
		return *this;
	}

	object_const_iterator object_const_iterator::operator--(int)
	{
		object_const_iterator old = *this;
		--(*this);
		return old;
	}

	object_const_iterator& object_const_iterator::operator-=(difference_type n)
	{
		object_ptr_it = static_cast<Ogre::MovableObject const* const*>(object_ptr_it) - n;
		return *this;
	}

	object_const_iterator object_const_iterator::operator-(difference_type n) const
	{
		object_const_iterator it = *this;
		it -= n;
		return it;
	}

	auto object_const_iterator::operator-(object_const_iterator const& rhs) const noexcept -> difference_type
	{
		return static_cast<Ogre::MovableObject const* const*>(object_ptr_it) - static_cast<Ogre::MovableObject const* const*>(rhs.object_ptr_it);
	}

	auto object_const_iterator::operator*() const -> element_type
	{
		return make_object_cref(**static_cast<Ogre::MovableObject const* const*>(object_ptr_it));
	}

	auto object_const_iterator::operator->() const -> pointer
	{
		return pointer{ *(*this) };
	}

	auto object_const_iterator::operator[](difference_type n) const -> reference
	{
		return make_object_cref(*static_cast<Ogre::MovableObject const* const*>(object_ptr_it)[n]);
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

	std::optional<node_cref> object_cref::get_owner() const noexcept
	{
		Ogre::SceneNode const* owner = get_object(*this).getParentSceneNode();
		if (owner == nullptr)
			return std::nullopt;
		return make_node_cref(*owner);
	}

	std::optional<node_ref> object_ref::get_owner() const noexcept
	{
		Ogre::SceneNode* const owner = get_object(*this).getParentSceneNode();
		if (owner == nullptr)
			return std::nullopt;
		return make_node_ref(*owner);
	}
}
