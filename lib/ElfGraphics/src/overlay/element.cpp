#include "overlay/element.h"

#include "Ogre/Components/Overlay/Manager.h"

namespace ot::egfx::overlay
{
	namespace detail
	{
		void init_element_impl(element& e, void* ptr) noexcept
		{
			e.pimpl = ptr;
		}

		void const* get_element_impl(element const& e) noexcept
		{
			return e.pimpl;
		}
	}

	void init_element(element& e, Ogre::v1::OverlayElement* p) noexcept
	{
		detail::init_element_impl(e, p);
	}

	Ogre::v1::OverlayElement& get_element(element& e) noexcept
	{
		return *static_cast<Ogre::v1::OverlayElement*>(const_cast<void*>(detail::get_element_impl(e)));
	}

	Ogre::v1::OverlayElement const& get_element(element const& e) noexcept
	{
		return *static_cast<Ogre::v1::OverlayElement const*>(detail::get_element_impl(e));
	}

	element::element() noexcept
	{
		init_element(*this, nullptr);
	}

	element::element(element&& other) noexcept
	{
		init_element(*this, static_cast<Ogre::v1::OverlayElement*>(other.pimpl));
		init_element(other, nullptr);
	}

	element& element::operator=(element&& other) noexcept
	{
		if (this != &other)
		{
			destroy_element();
			init_element(*this, static_cast<Ogre::v1::OverlayElement*>(other.pimpl));
			init_element(other, nullptr);
		}
		return *this;
	}

	element::~element()
	{
		destroy_element();
	}

	void element::destroy_element() noexcept
	{
		auto const ptr = static_cast<Ogre::v1::OverlayElement*>(pimpl);
		if (ptr != nullptr)
		{
			Ogre::v1::OverlayManager::getSingleton().destroyOverlayElement(ptr);
		}
	}

	void element::show()
	{
		get_element(*this).show();
	}

	void element::hide()
	{
		get_element(*this).hide();
	}

	[[nodiscard]] bool element::is_visible() const noexcept
	{
		return get_element(*this).isVisible();
	}

	void element::set_position(double left, double top)
	{
		get_element(*this).setPosition(static_cast<Ogre::Real>(left), static_cast<Ogre::Real>(top));
	}
}
