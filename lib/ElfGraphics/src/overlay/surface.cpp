#include "overlay/surface.h"

#include "overlay/container.h"

#include "Ogre/Components/Overlay/Manager.h"

namespace ot::egfx::overlay
{
	namespace detail
	{
		void init_surface_impl(surface& s, void* ptr) noexcept
		{
			s.pimpl = ptr;
		}

		void const* get_surface_impl(surface const& s) noexcept
		{
			return s.pimpl;
		}
	}

	void init_surface(surface& s, Ogre::v1::Overlay* p) noexcept
	{
		detail::init_surface_impl(s, p);
	}

	Ogre::v1::Overlay& get_surface(surface& s) noexcept
	{
		return *static_cast<Ogre::v1::Overlay*>(const_cast<void*>(detail::get_surface_impl(s)));
	}

	Ogre::v1::Overlay const& get_surface(surface const& s) noexcept
	{
		return *static_cast<Ogre::v1::Overlay const*>(detail::get_surface_impl(s));
	}

	surface create_surface(std::string const& name)
	{
		surface s;
		init_surface(s, Ogre::v1::OverlayManager::getSingleton().create(name));
		return s;
	}

	void surface::add(container& c)
	{
		get_surface(*this).add2D(&get_container(c));
	}

	void surface::show()
	{
		get_surface(*this).show();
	}

	void surface::hide()
	{
		get_surface(*this).hide();
	}

	bool surface::is_visible() const noexcept
	{
		return get_surface(*this).isVisible();
	}
}
