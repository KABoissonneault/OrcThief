#include "egfx/imgui/texture.h"

#include "imgui/renderer.h"

namespace ot::egfx::imgui
{
	texture::texture(texture&& rhs) noexcept
		: texture_id(rhs.texture_id)
		, width(rhs.width)
		, height(rhs.height)
		, renderer_system(rhs.renderer_system)
	{
		rhs.texture_id = nullptr;
	}

	texture& texture::operator=(texture&& rhs) noexcept
	{
		if (this != &rhs)
		{
			texture_id = rhs.texture_id;
			width = rhs.width;
			height = rhs.height;
			renderer_system = rhs.renderer_system;

			rhs.texture_id = nullptr;
		}

		return *this;
	}

	texture::~texture()
	{
		if (renderer_system != nullptr)
			renderer_system->free_texture(*this);
	}
}
