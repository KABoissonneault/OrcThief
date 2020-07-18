#include "Ogre/Components/Overlay/PanelElement.h"

#include "Ogre/Components/Overlay/Manager.h"

namespace ot::ogre::overlay
{
	namespace type
	{
		const string panel("Panel");
	}

	Ogre::v1::PanelOverlayElement* make_panel_element(const string& instance_name)
	{
		return static_cast<Ogre::v1::PanelOverlayElement*>(Ogre::v1::OverlayManager::getSingleton().createOverlayElement(type::panel, instance_name));
	}
}
