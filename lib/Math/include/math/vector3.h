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

		[[nodiscard]] T norm() const
		{
			return static_cast<T>(std::sqrt(x * x + y * y + z * z));
		}

		[[nodiscard]] constexpr T norm_squared() const noexcept
		{
			return x * x + y * y + z * z;
		}

		[[nodiscard]] static vector3 unit_x() noexcept { return { 1, 0, 0 }; }
		[[nodiscard]] static vector3 unit_y() noexcept { return { 0, 1, 0 }; }
		[[nodiscard]] static vector3 unit_z() noexcept { return { 0, 0, 1 }; }
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
	constexpr inline vector3<T>& operator+=(vector3<T>& lhs, vector3<T> rhs) noexcept
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		lhs.z += rhs.z;
		return lhs;
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> operator-(vector3<T> v) noexcept
	{
		return { -v.x, -v.y, -v.z };
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

	template<typename T>
	[[nodiscard]] constexpr inline point3<T> destination_from_origin(vector3<T> v) noexcept
	{
		return { v.x, v.y, v.z };
	}

	template<typename T>
	[[nodiscard]] constexpr inline vector3<T> vector_from_origin(point3<T> v) noexcept
	{
		return { v.x, v.y, v.z };
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

	using ot::float_eq;

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

	// Returns the distance between the point p0 and the line represented by p1 and p2
	[[nodiscard]] inline double distance_point_to_line(point3f p0, point3f p1, point3f p2)
	{
		return cross_product(p2 - p1, p1 - p0).norm() / (p2 - p1).norm();
	}

	// Returns the distance between the point p0 and the line represented by p1 and p2
	[[nodiscard]] inline double distance_point_to_line(point3d p0, point3d p1, point3d p2)
	{
		return cross_product(p2 - p1, p1 - p0).norm() / (p2 - p1).norm();
	}

	// Returns the 'inverse' of the input vector
	//
	// This is the logical equivalent of v^-1, not -v. Useful for "dividing" vectors
	[[nodiscard]] inline vector3f inverse(vector3f v)
	{
		return v / v.norm_squared();
	}

	// Returns the 'inverse' of the input vector
	//
	// This is the logical equivalent of v^-1, not -v. Useful for "dividing" vectors
	[[nodiscard]] inline vector3d inverse(vector3d v)
	{
		return v / v.norm_squared();
	}
}