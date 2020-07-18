#pragma once

#include "Ogre/detail/common.h"
#include "Ogre/Prerequisites.h"

OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN
#include <OgreTextAreaOverlayElement.h>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END

namespace ot::ogre::overlay
{
	namespace type
	{
		extern const string text_area;
	}

	Ogre::v1::TextAreaOverlayElement* make_text_area_element(const string& instance_name);
}