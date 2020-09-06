#include "overlay/panel.h"

namespace ot::egfx::overlay
{
	void init_panel(panel& e, Ogre::v1::PanelOverlayElement* p) noexcept
	{
		init_element(e, p);
	}

	Ogre::v1::PanelOverlayElement& get_panel(panel& e) noexcept
	{
		return static_cast<Ogre::v1::PanelOverlayElement&>(get_element(e));
	}

	Ogre::v1::PanelOverlayElement const& get_panel(panel const& e) noexcept
	{
		return static_cast<Ogre::v1::PanelOverlayElement const&>(get_element(e));
	}

	panel create_panel(std::string const& name)
	{
		panel p;
		init_panel(p, ogre::overlay::make_panel_element(name));
		return p;
	}
}
