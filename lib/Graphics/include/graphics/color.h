#pragma once

namespace ot::graphics
{
	struct color
	{
		double r, g, b, a;

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
