#pragma once

#include "Ogre/detail/common.h"
#include "Ogre/Prerequisites.h"

OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN
#include <OgrePanelOverlayElement.h>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END

namespace ot::ogre::overlay
{
	namespace type
	{
		extern const string panel;
	}

	Ogre::v1::PanelOverlayElement* make_panel_element(const string& instance_name);
}
