#include "serialize_mesh_definition.h"

#include "egfx/mesh_definition.h"

#include "serialize_math.h"

#include <iostream>

namespace ot::dedit
{
	std::ostream& operator<<(std::ostream& o, egfx::mesh_definition const& m)
	{
		auto const faces = m.get_faces();
		o << faces.size();
		for (egfx::face::cref const f : faces)
		{
			o << f.get_plane();
		}
		return o;
	}

	std::istream& operator>>(std::istream& i, egfx::mesh_definition& m)
	{
		size_t face_count;
		if (i >> face_count)
		{
			std::vector<math::plane> v(face_count);
			for (math::plane& p : v)
			{
				i >> p;
			}

			if (i)
			{
				m = egfx::mesh_definition(v);
			}
		}
		return i;
	}
}
