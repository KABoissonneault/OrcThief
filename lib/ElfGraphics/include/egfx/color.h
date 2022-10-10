#pragma once

namespace ot::egfx
{
	struct color
	{
		float r, g, b, a = 1.0f;

		static constexpr color white() noexcept
		{
			return { 1, 1, 1, 1 };
		}

		static constexpr color black() noexcept
		{
			return { 0, 0, 0, 1 };
		}
	};
}
