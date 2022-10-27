#pragma once

#include "imgui/renderer.h"

struct ID3D11Device;

namespace ot::egfx::imgui
{
	class d3d11_renderer : public renderer
	{
		ID3D11Device* d3d_device;
	public:
		virtual bool initialize() override;
		virtual void shutdown() override;
		virtual void render(Ogre::Camera const* camera) override;

		virtual bool load_texture(std::span<unsigned char> image_rgba_data, int pitch, texture& t) override;
		virtual void free_texture(texture& t) override;
	};
}