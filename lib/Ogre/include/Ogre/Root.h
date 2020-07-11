#pragma once

#include "Ogre/detail/common.h"
#include "Ogre/Prerequisites.h"

OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN
#include <OgreRoot.h>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END

namespace ot::ogre
{
	using name_value_pair_list = Ogre::NameValuePairList;

	// Helper to fill Ogre::Root::createRenderWindow's misc params
	// Full list of params can be found here: https://ogrecave.github.io/ogre-next/api/2.2/class_ogre_1_1_root.html#ae1c38392ce62547652f39b9a899fb20a
	struct render_window_misc_params
	{
		name_value_pair_list params;

		// Sets a handle to an existing physical window. Use this if you don't want Ogre to create a new window
		auto set_external_window_handle(string value) -> render_window_misc_params&;

		// Sets whether "sRGB Gamma Conversion" should be used
		auto set_hardware_gamma_conversion(bool value) -> render_window_misc_params&;

		// Sets whether vsync should be used
		auto set_vsync(bool value) -> render_window_misc_params&;
	};
}
