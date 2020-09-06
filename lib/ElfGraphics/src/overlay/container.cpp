#include "overlay/container.h"

namespace ot::egfx::overlay
{
	void init_container(container& e, Ogre::v1::OverlayContainer* ptr) noexcept
	{
		init_element(e, ptr);
	}

	Ogre::v1::OverlayContainer& get_container(container& e) noexcept
	{
		return static_cast<Ogre::v1::OverlayContainer&>(get_element(e));
	}

	Ogre::v1::OverlayContainer const& get_container(container const& e) noexcept
	{
		return static_cast<Ogre::v1::OverlayContainer const&>(get_element(e));
	}

	void container::add_child(element& c)
	{
		get_container(*this).addChild(&get_element(c));
	}
}
