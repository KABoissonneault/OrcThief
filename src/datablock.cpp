#include "datablock.h"

#include "Ogre/Root.h"
#include "Ogre/HlmsManager.h"
#include "Ogre/Components/Hlms/Unlit.h"
#include "Ogre/Components/Hlms/Unlit/Datablock.h"

namespace ot::datablock
{
	std::string const overlay_unlit("OverlayUnlit");

	Ogre::HlmsMacroblock overlay_macro;

	void initialize()
	{
		Ogre::Root& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager& hlms_manager = *root.getHlmsManager();
		Ogre::HlmsUnlit& hlms_unlit = static_cast<Ogre::HlmsUnlit&>(*hlms_manager.getHlms(Ogre::HLMS_UNLIT));

		overlay_macro.mDepthBiasConstant = 0.1f;
		
		auto const default_unlit_block = static_cast<Ogre::HlmsUnlitDatablock*>(
			hlms_unlit.createDatablock(overlay_unlit, overlay_unlit, overlay_macro, Ogre::HlmsBlendblock(), Ogre::HlmsParamVec())
		);

		default_unlit_block->setUseColour(true);
		default_unlit_block->setColour(Ogre::ColourValue::White);
	}
}