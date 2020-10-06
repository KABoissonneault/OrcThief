#include "serialize_math.h"

#include <iostream>

namespace ot::dedit
{
	std::ostream& operator<<(std::ostream& o, math::point3f const& p)
	{
		o << p.x;
		o << p.y;
		o << p.z;
		return o;
	}

	std::istream& operator>>(std::istream& i, math::point3f& p)
	{
		(i >> p.x) && (i >> p.y) && (i >> p.z);
		return i;
	}

	std::ostream& operator<<(std::ostream& o, math::vector3f const& p)
	{
		o << p.x;
		o << p.y;
		o << p.z;
		return o;
	}

	std::istream& operator>>(std::istream& i, math::vector3f& p)
	{
		(i >> p.x) && (i >> p.y) && (i >> p.z);
		return i;
	}

	std::ostream& operator<<(std::ostream& o, math::plane const& p)
	{
		o << p.distance;
		o << p.normal;

		return o;
	}

	std::istream& operator>>(std::istream& i, math::plane& p)
	{
		(i >> p.distance) && (i >> p.normal);
		return i;
	}

	std::ostream& operator<<(std::ostream& o, math::quaternion const& p)
	{
		o << p.w;
		o << p.x;
		o << p.y;
		o << p.z;
		return o;
	}

	std::istream& operator>>(std::istream& i, math::quaternion& p)
	{
		(i >> p.w) && (i >> p.x) && (i >> p.y) && (i >> p.z);
		return i;
	}
}
