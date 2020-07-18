#pragma once

#include <chrono>

namespace ot::math
{
	using seconds = std::chrono::duration<double>;
	using milliseconds = std::chrono::duration<double, std::ratio<1, 1000>>;

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
