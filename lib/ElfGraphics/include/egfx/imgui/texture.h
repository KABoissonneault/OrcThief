#pragma once

namespace ot::egfx::imgui
{
	class renderer;

	class texture
	{
		void* texture_id = nullptr;
		int width;
		int height;
		renderer* renderer_system;

		friend class d3d11_renderer;
	public:
		texture() = default;
		texture(const texture&) = delete;
		texture& operator=(const texture&) = delete;
		texture(texture&& rhs) noexcept;
		texture& operator=(texture&& rhs) noexcept;
		~texture();

		[[nodiscard]] void* get_texture_id() const noexcept { return texture_id; }
		[[nodiscard]] int get_width() const noexcept { return width; }
		[[nodiscard]] int get_height() const noexcept { return height; }
	};
}
