#pragma once

#include <chrono>

namespace ot::math
{
	using seconds = std::chrono::duration<float>;
	using milliseconds = std::chrono::duration<float, std::ratio<1, 1000>>;

	namespace literals
	{
		inline constexpr seconds operator""_s(long double s) noexcept
		{
			return seconds(s);
		}

		inline constexpr milliseconds operator""_ms(long double s) noexcept
		{
			return milliseconds(s);
		}
	}
}
