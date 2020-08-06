#pragma once

#include <cmath>
#include <cfloat>

#include <cstdio>

namespace ot
{
	namespace detail
	{
		template<typename Float>
		inline int float_cmp(Float lhs, Float rhs, Float epsilon);

		extern template int float_cmp(float, float, float);
		extern template int float_cmp(double, double, double);
	}

	// Compares two floats, lhs and rhs, and return:
	//   Negative if lhs is clearly smaller than rhs
	//   Positive if lhs is clearly larger than rhs
	//   0 if lhs is almost equal to rhs
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload uses FLT_EPSILON as the epsilon value
	inline int float_cmp(float lhs, float rhs)
	{
		return detail::float_cmp(lhs, rhs, FLT_EPSILON);
	}

	// Compares two floats, lhs and rhs, and return:
	//   Negative if lhs is clearly smaller than rhs
	//   Positive if lhs is clearly larger than rhs
	//   0 if lhs is almost equal to rhs
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload computes epsilon with (pow(2, bits_error) - 1) * FLT_EPSILON
	// bits_error represents the number of bits of "error" in the computation
	inline int float_cmp(float lhs, float rhs, int bits_error)
	{
		float const epsilon = (static_cast<float>(std::pow(2, bits_error)) - 1.0f) * FLT_EPSILON;
		return detail::float_cmp(lhs, rhs, epsilon);
	}

	// Compares two floats, lhs and rhs, and returns true if they are almost equal, false otherwise
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload uses FLT_EPSILON as the epsilon value
	inline bool float_eq(float lhs, float rhs)
	{
		return float_cmp(lhs, rhs) == 0;
	}

	// Compares two floats, lhs and rhs, and returns true if they are almost equal, false otherwise
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload computes epsilon with (pow(2, bits_error) - 1) * FLT_EPSILON
	// bits_error represents the number of bits of "error" in the computation
	inline bool float_eq(float lhs, float rhs, int bits_error)
	{
		return float_cmp(lhs, rhs, bits_error) == 0;
	}

	// Compares two floats, lhs and rhs, and return:
	//   Negative if lhs is clearly smaller than rhs
	//   Positive if lhs is clearly larger than rhs
	//   0 if lhs is almost equal to rhs
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload uses DBL_EPSILON as the epsilon value
	inline int float_cmp(double lhs, double rhs)
	{
		return detail::float_cmp(lhs, rhs, DBL_EPSILON);
	}

	// Compares two floats, lhs and rhs, and return:
	//   Negative if lhs is clearly smaller than rhs
	//   Positive if lhs is clearly larger than rhs
	//   0 if lhs is almost equal to rhs
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload computes epsilon with (pow(2, bits_error) - 1) * DBL_EPSILON
	// bits_error represents the number of bits of "error" in the computation
	inline int float_cmp(double lhs, double rhs, int bits_error)
	{
		double const epsilon = (std::pow(2, bits_error) - 1.0) * DBL_EPSILON;
		return detail::float_cmp(lhs, rhs, epsilon);
	}

	// Compares two floats, lhs and rhs, and returns true if they are almost equal, false otherwise
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload uses DBL_EPSILON as the epsilon value
	inline bool float_eq(double lhs, double rhs)
	{
		return float_cmp(lhs, rhs) == 0;
	}

	// Compares two floats, lhs and rhs, and returns true if they are almost equal, false otherwise
	//
	// The maximum tolerance is determined by an "epsilon" value raised to the largest exponent of the two arguments
	// This overload computes epsilon with (pow(2, bits_error) - 1) * DBL_EPSILON
	// bits_error represents the number of bits of "error" in the computation
	inline bool float_eq(double lhs, double rhs, int bits_error)
	{
		return float_cmp(lhs, rhs, bits_error) == 0;
	}
}