#include "datablock.h"

#include "Ogre/Root.h"
#include "Ogre/HlmsManager.h"
#include "Ogre/Components/Hlms/Unlit.h"
#include "Ogre/Components/Hlms/Unlit/Datablock.h"

#include <cfloat>

namespace ot::datablock
{
	std::string const overlay_unlit("OverlayUnlit");	
	std::string const overlay_unlit_vertex("OverlayUnlitVertex");	
	std::string const overlay_unlit_vertex_transparent("OverlayUnlitVertexTransparent");
	std::string const overlay_unlit_edge("OverlayUnlitEdge");	
	std::string const overlay_unlit_transparent_light("OverlayUnlitTransparentLight");
	std::string const overlay_unlit_transparent_heavy("OverlayUnlitTransparentHeavy");

	void initialize()
	{
		Ogre::Root& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager& hlms_manager = *root.getHlmsManager();
		Ogre::HlmsUnlit& hlms_unlit = static_cast<Ogre::HlmsUnlit&>(*hlms_manager.getHlms(Ogre::HLMS_UNLIT));

		Ogre::HlmsMacroblock overlay_macro;
		overlay_macro.mDepthCheck = false;

		Ogre::HlmsBlendblock overlay_blend;
		overlay_blend.mIsTransparent = 1;
		overlay_blend.setBlendType(Ogre::SBT_TRANSPARENT_ALPHA);

		auto make_colored_block = [&hlms_unlit, &overlay_macro](std::string const& name, Ogre::HlmsBlendblock const& blend_block, Ogre::ColourValue color)
		{
			auto const block = static_cast<Ogre::HlmsUnlitDatablock*>(
				hlms_unlit.createDatablock(name, name, Ogre::HlmsMacroblock(), blend_block, Ogre::HlmsParamVec())
				);

			block->setUseColour(true);
			block->setColour(color);
		};

		auto make_colored_overlay_block = [&hlms_unlit, &overlay_macro](std::string const& name, Ogre::HlmsBlendblock const& blend_block, Ogre::ColourValue color)
		{
			auto const block = static_cast<Ogre::HlmsUnlitDatablock*>(
				hlms_unlit.createDatablock(name, name, overlay_macro, blend_block, Ogre::HlmsParamVec())
				);

			block->setUseColour(true);
			block->setColour(color);
		};

		make_colored_block(overlay_unlit_vertex, Ogre::HlmsBlendblock(), Ogre::ColourValue::Blue);
		make_colored_overlay_block(overlay_unlit, Ogre::HlmsBlendblock(), Ogre::ColourValue::White);
		make_colored_overlay_block(overlay_unlit_vertex_transparent, overlay_blend, Ogre::ColourValue::Blue - Ogre::ColourValue{ 0, 0, 0, 0.5f });
		make_colored_overlay_block(overlay_unlit_edge, Ogre::HlmsBlendblock(), Ogre::ColourValue::Red);
		make_colored_overlay_block(overlay_unlit_transparent_light, overlay_blend, Ogre::ColourValue{ 1.0f, 1.0f, 1.0f, 0.25f });
		make_colored_overlay_block(overlay_unlit_transparent_heavy, overlay_blend, Ogre::ColourValue{ 1.0f, 1.0f, 1.0f, 0.6f });
	}
}
