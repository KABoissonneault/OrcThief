#pragma once

#include "imgui/renderer.h"

namespace ot::graphics::imgui
{
	class d3d11_renderer : public renderer
	{
	public:
		virtual bool initialize() override;
		virtual void shutdown() override;
		virtual void new_frame() override;
		virtual void render() override;
	};
}