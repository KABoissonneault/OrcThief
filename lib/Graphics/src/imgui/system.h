#pragma once

#include "core/uptr.h"
#include "imgui/renderer.h"

namespace Ogre { class CompositorWorkspaceDef; }

namespace ot::graphics::imgui
{
	class pass_provider;

	class system
	{
		uptr<renderer> render;
		fwd_uptr<pass_provider> provider;

	public:
		bool initialize(Ogre::CompositorWorkspaceDef* def);
		void shutdown();
		void new_frame();
	};

}

extern template struct ot::fwd_delete<ot::graphics::imgui::pass_provider>;
