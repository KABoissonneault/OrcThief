#pragma once

#include "core/float.h"

#include <numeric>
#include <algorithm>

namespace ot::math
{
	template<typename T>
	struct point3
	{
		T x, y, z;
	};

	template<typename T>
	[[nodiscard]] constexpr bool operator==(point3<T> lhs, point3<T> rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	template<typename T>
	struct vector3
	{
		T x, y, z;

		T norm() const
		{
			return static_cast<T>(std::sqrt(x * x + y * y + z * z));
		}
	};

	template<typename T>
	[[nodiscard]] constexpr inline point3<T> operator+(point3<T> lhs, vector3<T> rhs) noexcept
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> operator+(vector3<T> lhs, vector3<T> rhs) noexcept
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}

	template<typename T>
	constexpr inline point3<T>& operator+=(point3<T>& lhs, vector3<T> rhs) noexcept
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		return lhs;
	}

	template<typename T>
	constexpr inline point3<T>& operator+=(vector3<T>& lhs, vector3<T> rhs) noexcept
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline point3<T> operator-(point3<T> lhs, vector3<T> rhs) noexcept
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point3<T>& operator-=(point3<T>& lhs, vector3<T> rhs) noexcept
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		lhs.z -= rhs.z;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> operator-(point3<T> lhs, point3<T> rhs) noexcept
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> operator*(vector3<T> lhs, T rhs) noexcept
	{
		return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
	}

	template<typename T>
	constexpr inline vector3<T>& operator*=(vector3<T>& lhs, T rhs) noexcept
	{
		lhs.x *= rhs;
		lhs.y *= rhs;
		lhs.z *= rhs;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> operator/(vector3<T> lhs, T rhs) noexcept
	{
		return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
	}

	template<typename T>
	constexpr inline vector3<T>& operator/=(vector3<T>& lhs, T rhs) noexcept
	{
		lhs.x /= rhs;
		lhs.y /= rhs;
		lhs.z /= rhs;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> cross_product(vector3<T> lhs, vector3<T> rhs) noexcept
	{
		return
		{
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	template<typename T>
	[[nodiscard]] constexpr inline T dot_product(vector3<T> lhs, vector3<T> rhs) noexcept
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	template<typename T>
	[[nodiscard]] constexpr inline point3<T> component_min(point3<T> lhs, point3<T> rhs) noexcept
	{
		return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z) };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point3<T> component_max(point3<T> lhs, point3<T> rhs) noexcept
	{
		return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z) };
	}

	template<typename T>
	[[nodiscard]] constexpr inline point3<T> midpoint(point3<T> lhs, point3<T> rhs) noexcept
	{
		return { std::midpoint(lhs.x, rhs.x), std::midpoint(lhs.y, rhs.y), std::midpoint(lhs.z, rhs.z) };
	}

	using point3i = point3<int>;
	using point3f = point3<float>;
	using point3d = point3<double>;
	using vector3i = vector3<int>;
	using vector3f = vector3<float>;
	using vector3d = vector3<double>;

	inline vector3f normalized(vector3f v)
	{
		return v / v.norm();
	}

	inline vector3d normalized(vector3d v)
	{
		return v / v.norm();
	}

	inline bool is_normalized(vector3f v)
	{
		return ot::float_eq(v.norm(), 1.0f);
	}

	inline bool is_normalized(vector3d v)
	{
		return ot::float_eq(v.norm(), 1.0);
	}

	[[nodiscard]] inline bool float_eq(point3f lhs, point3f rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y) && ot::float_eq(lhs.z, rhs.z);
	}

	[[nodiscard]] inline bool float_eq(vector3f lhs, vector3f rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y) && ot::float_eq(lhs.z, rhs.z);
	}

	[[nodiscard]] inline bool float_eq(point3d lhs, point3d rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y) && ot::float_eq(lhs.z, rhs.z);
	}

	[[nodiscard]] inline bool float_eq(vector3d lhs, vector3d rhs) noexcept
	{
		return ot::float_eq(lhs.x, rhs.x) && ot::float_eq(lhs.y, rhs.y) && ot::float_eq(lhs.z, rhs.z);
	}


}