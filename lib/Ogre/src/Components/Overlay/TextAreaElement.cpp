#include "Ogre/Components/Overlay/TextAreaElement.h"

#include "Ogre/Components/Overlay/Manager.h"

namespace ot::ogre::overlay
{
	namespace type
	{
		const string text_area("TextArea");
	}

	Ogre::v1::TextAreaOverlayElement* make_text_area_element(const string& instance_name)
	{
		return static_cast<Ogre::v1::TextAreaOverlayElement*>(Ogre::v1::OverlayManager::getSingleton().createOverlayElement(type::text_area, instance_name));
	}
}