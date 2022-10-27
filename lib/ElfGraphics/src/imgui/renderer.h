#pragma once

#include "Ogre/Prerequisites.h"

#include "egfx/imgui/texture.h"

#include <span>

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

		virtual bool load_texture(std::span<unsigned char> image_rgba_data, int pitch, texture& t) = 0;
		virtual void free_texture(texture& t) = 0;
	};
}
