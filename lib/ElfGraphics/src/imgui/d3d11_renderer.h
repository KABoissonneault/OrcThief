#pragma once

#include "imgui/renderer.h"

namespace ot::egfx::imgui
{
	class d3d11_renderer : public renderer
	{
	public:
		virtual bool initialize() override;
		virtual void shutdown() override;
		virtual void render(Ogre::Camera const* camera) override;
	};
}