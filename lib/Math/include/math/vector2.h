#pragma once

#include "core/float.h"

#include <numeric>
#include <algorithm>

namespace ot::math
{
	template<typename T>
	struct point2
	{
		T x, y;
	};

	template<typename T>
	[[nodiscard]] constexpr bool operator==(point2<T> lhs, point2<T> rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	template<typename T>
	struct vector2
	{
		T x, y;

		[[nodiscard]] T norm() const
		{
			return static_cast<T>(std::sqrt(norm_squared()));
		}

		[[nodiscard]] constexpr T norm_squared() const noexcept
		{
			return x * x + y * y;
		}

		[[nodiscard]] static vector2 unit_x() noexcept { return { 1, 0, 0 }; }
		[[nodiscard]] static vector2 unit_y() noexcept { return { 0, 1, 0 }; }
		[[nodiscard]] static vector2 unit_z() noexcept { return { 0, 0, 1 }; }
	};

	template<typename T>
	[[nodiscard]] constexpr inline point2<T> operator+(point2<T> lhs, vector2<T> rhs) noexcept
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector2<T> operator+(vector2<T> lhs, vector2<T> rhs) noexcept
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	template<typename T>
	constexpr inline point2<T>& operator+=(point2<T>& lhs, vector2<T> rhs) noexcept
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		return lhs;
	}

	template<typename T>
	constexpr inline vector2<T>& operator+=(vector2<T>& lhs, vector2<T> rhs) noexcept
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector2<T> operator-(vector2<T> v) noexcept
	{
		return { -v.x, -v.y };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point2<T> operator-(point2<T> lhs, vector2<T> rhs) noexcept
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point2<T>& operator-=(point2<T>& lhs, vector2<T> rhs) noexcept
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector2<T> operator-(point2<T> lhs, point2<T> rhs) noexcept
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector2<T> operator*(vector2<T> lhs, T rhs) noexcept
	{
		return { lhs.x * rhs, lhs.y * rhs };
	}

	template<typename T>
	constexpr inline vector2<T>& operator*=(vector2<T>& lhs, T rhs) noexcept
	{
		lhs.x *= rhs;
		lhs.y *= rhs;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector2<T> operator/(vector2<T> lhs, T rhs) noexcept
	{
		return { lhs.x / rhs, lhs.y / rhs };
	}

	template<typename T>
	constexpr inline vector2<T>& operator/=(vector2<T>& lhs, T rhs) noexcept
	{
		lhs.x /= rhs;
		lhs.y /= rhs;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline T dot_product(vector2<T> lhs, vector2<T> rhs) noexcept
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	template<typename T>
	[[nodiscard]] constexpr inline point2<T> component_min(point2<T> lhs, point2<T> rhs) noexcept
	{
		return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y) };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point2<T> component_max(point2<T> lhs, point2<T> rhs) noexcept
	{
		return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y) };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point2<T> midpoint(point2<T> lhs, point2<T> rhs) noexcept
	{
		return { std::midpoint(lhs.x, rhs.x), std::midpoint(lhs.y, rhs.y) };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point2<T> destination_from_origin(vector2<T> v) noexcept
	{
		return { v.x, v.y };
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector2<T> vector_from_origin(point2<T> v) noexcept
	{
		return { v.x, v.y, v.z };
	}

	using point2i = point2<int>;
	using point2f = point2<float>;
	using point2d = point2<double>;
	using vector2i = vector2<int>;
	using vector2f = vector2<float>;
	using vector2d = vector2<double>;

	inline vector2f normalized(vector2f v)
	{
		return v / v.norm();
	}

	inline vector2d normalized(vector2d v)
	{
		return v / v.norm();
	}

	inline bool is_normalized(vector2f v)
	{
		return ot::float_eq(v.norm(), 1.0f);
	}

	inline bool is_normalized(vector2d v)
	{
		return ot::float_eq(v.norm(), 1.0);
	}

	[[nodiscard]] inline bool float_eq(point2f lhs, point2f rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y);
	}

	[[nodiscard]] inline bool float_eq(vector2f lhs, vector2f rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y);
	}

	[[nodiscard]] inline bool float_eq(point2d lhs, point2d rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y);
	}

	[[nodiscard]] inline bool float_eq(vector2d lhs, vector2d rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y);
	}

	// Returns the 'inverse' of the input vector
	//
	// This is the logical equivalent of v^-1, not -v. Useful for "dividing" vectors
	[[nodiscard]] inline vector2f inverse(vector2f v)
	{
		return v / v.norm_squared();
	}

	// Returns the 'inverse' of the input vector
	//
	// This is the logical equivalent of v^-1, not -v. Useful for "dividing" vectors
	[[nodiscard]] inline vector2d inverse(vector2d v)
	{
		return v / v.norm_squared();
	}
}