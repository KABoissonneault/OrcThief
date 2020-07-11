#pragma once

#include "Ogre/detail/common.h"
#include "Ogre/ConfigOptionMap.h"

OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN
#include <OgreRenderSystem.h>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END

namespace ot::ogre::render_system
{
	bool get_fullscreen(config_option_map const& config, bool& result);
	bool get_resolution(config_option_map const& config, int& width, int& height);
	bool get_hardware_gamma_conversion(config_option_map const& config, bool& result);
	bool get_vsync(config_option_map const& config, bool& result);
}