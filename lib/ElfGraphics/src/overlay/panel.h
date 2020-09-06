#pragma once

#include "egfx/overlay/panel.h"
#include "overlay/element.h"
#include "Ogre/Components/Overlay/PanelElement.h"

namespace ot::egfx::overlay
{
	void init_panel(panel& e, Ogre::v1::PanelOverlayElement* p) noexcept;
	Ogre::v1::PanelOverlayElement& get_panel(panel& e) noexcept;
	Ogre::v1::PanelOverlayElement const& get_panel(panel const& e) noexcept;
}
