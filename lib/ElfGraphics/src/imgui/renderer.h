#pragma once

#include "Ogre/Prerequisites.h"

namespace ot::egfx::imgui
{
	class renderer
	{
	public:
		virtual ~renderer() = 0;

		virtual bool initialize() = 0;
		virtual void shutdown() = 0;
		virtual void pre_update(Ogre::Camera const* camera);
		virtual void render(Ogre::Camera const* camera);
	};
}
