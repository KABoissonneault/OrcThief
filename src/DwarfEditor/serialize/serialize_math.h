#pragma once

#include "math/vector3.h"
#include "math/plane.h"
#include "math/quaternion.h"

#include <iosfwd>

namespace ot::dedit
{
	std::ostream& operator<<(std::ostream& o, math::point3f const& p);
	std::istream& operator>>(std::istream& i, math::point3f& p);
	std::ostream& operator<<(std::ostream& o, math::vector3f const& p);
	std::istream& operator>>(std::istream& i, math::vector3f& p);
	std::ostream& operator<<(std::ostream& o, math::plane const& p);
	std::istream& operator>>(std::istream& i, math::plane& p);
	std::ostream& operator<<(std::ostream& o, math::quaternion const& p);
	std::istream& operator>>(std::istream& i, math::quaternion& p);

}
