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

			int lexp;
			int rexp;
			Float const lfrac = std::frexp(lhs, &lexp);
			Float const rfrac = std::frexp(rhs, &rexp);

			Float minscaled;
			Float maxfrac;
			if (lexp < rexp)
			{
				minscaled = std::ldexp(lfrac, lexp - rexp);
				maxfrac = rfrac;
			} 
			else
			{
				minscaled = std::ldexp(rfrac, rexp - lexp);
				maxfrac = lfrac;
			}

			Float const difference = maxfrac - minscaled;

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
