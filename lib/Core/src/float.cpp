#include "core/float.h"

namespace ot
{
	namespace detail
	{
		template<typename Float>
		inline int float_cmp(Float lhs, Float rhs, Float epsilon)
		{
			// frexpr won't behave as expected if either value is 0
			if (lhs == Float(0) || rhs == Float(0))
			{
				auto const difference = lhs - rhs;
				if (difference > epsilon)
					return 1;
				else if (difference < -epsilon)
					return -1;
				else
					return 0;
			}

			// Scale the smallest value to the same exponent as the largest one, and compare the difference in the "frac" part
			int lexp;
			int rexp;
			Float lfrac = std::frexp(lhs, &lexp);
			Float rfrac = std::frexp(rhs, &rexp);

			if (lexp < rexp)
			{
				lfrac = std::ldexp(lfrac, lexp - rexp);
			} else if (rexp < lexp)
			{
				rfrac = std::ldexp(rfrac, rexp - lexp);
			}

			Float const difference = lfrac - rfrac;

			if (difference > epsilon)
				return 1; // lhs > rhs
			else if (difference < -epsilon)
				return -1; // lhs < rhs
			else
				return 0; // lhs == rhs
		}

		template int float_cmp(float, float, float);
		template int float_cmp(double, double, double);
	}
}
