#pragma once

#include <type_traits>

namespace ot::math
{
	template<typename T, typename U>
	[[nodiscard]] inline constexpr std::common_type_t<T, U> min_value(T lhs, U rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}

	template<typename T, typename U>
	[[nodiscard]] inline constexpr std::common_type_t<T, U> max_value(T lhs, U rhs)
	{
		return lhs < rhs ? rhs : lhs;
	}
}
