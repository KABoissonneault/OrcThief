#include "serialize_mesh_definition.h"

#include "egfx/mesh_definition.h"

#include "serialize_math.h"

#include <iostream>

namespace ot::dedit::serialize
{
	bool fwrite(egfx::mesh_definition const& m, std::FILE* f)
	{
		auto const faces = m.get_faces();
		size_t const face_count = faces.size();
		if (::fwrite(&face_count, sizeof(face_count), 1, f) != 1)
			return false;

		std::vector<math::plane> v;
		v.reserve(faces.size());
		for (egfx::face::cref const fr : faces)
		{
			v.push_back(fr.get_plane());
		}

		return fwrite(v, f);
	}

	bool fread(egfx::mesh_definition& m, std::FILE* f)
	{
		size_t face_count;
		if (::fread(&face_count, sizeof(face_count), 1, f) != 1)
			return false;

		std::vector<math::plane> v(face_count);
		if (!fread(v, f))
			return false;

		m = egfx::mesh_definition(v);

		return true;
	}
}
