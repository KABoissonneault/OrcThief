#include "serialize_map.h"

#include "serialize_mesh_definition.h"
#include "serialize_math.h"

#include <iostream>

namespace ot::dedit::serialize
{
	bool fwrite(map const& m, std::FILE* f)
	{
		std::span<brush const> const brushes = m.get_brushes();
		size_t const brush_count = brushes.size();

		if (!::fwrite(&brush_count, sizeof(brush_count), 1, f))
			return false;

		for (brush const& b : brushes)
		{
			if (!fwrite(b.get_mesh_def(), f))
				return false;

			egfx::node::object_cref const node = b.get_node();
			if (
				!fwrite(node.get_position(), f)
				|| !fwrite(node.get_rotation(), f)
				|| !fwrite(node.get_scale(), f)
				)
				return false;
		}

		return true;
	}

	bool fread(map& m, std::FILE* f)
	{
		size_t brush_count;
		if (!::fread(&brush_count, sizeof(brush_count), 1, f))
			return false;

		for (size_t n = 0; n < brush_count; ++n)
		{
			auto mesh_def = std::make_shared<egfx::mesh_definition>();
			if (!fread(*mesh_def, f))
				return false;

			math::point3f position;
			math::quaternion rotation;
			math::scales scales;
			if (!fread(position, f) || !fread(rotation, f) || !fread(scales, f))
				return false;

			brush& b = m.make_brush(std::move(mesh_def), m.allocate_entity_id());
			egfx::node::object_ref const node = b.get_node();
			node.set_position(position);
			node.set_rotation(rotation);
			node.set_scale(scales);
		}

		return true;
	}
}
