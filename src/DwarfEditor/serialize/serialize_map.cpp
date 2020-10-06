#include "serialize_map.h"

#include "serialize_mesh_definition.h"
#include "serialize_math.h"

#include <iostream>

namespace ot::dedit
{

	std::ostream& operator<<(std::ostream& o, map const& m)
	{
		std::span<brush const> const brushes = m.get_brushes();
		
		o << brushes.size();
		for (brush const& b : brushes)
		{
			o << *b.mesh_def;
			egfx::node::object_cref const node = b.node;
			o << node.get_position();
			o << node.get_rotation();
			auto const scales = node.get_scale();
			o << scales.x << scales.y << scales.z;
		}

		return o;
	}

	std::istream& operator>>(std::istream& i, map& m)
	{
		size_t brush_count;
		if (i >> brush_count)
		{
			for (size_t n = 0; n < brush_count; ++n)
			{
				auto mesh_def = std::make_shared<egfx::mesh_definition>();
				if (!(i >> *mesh_def))
					break;

				math::point3f position;
				math::quaternion rotation;
				math::scales scales;

				if (!((i >> position) && (i >> rotation) && (i >> scales.x) && (i >> scales.y) && (i >> scales.z)))
					break;

				brush& b = m.make_brush(std::move(mesh_def), m.allocate_entity_id());
				egfx::node::object_ref const node = b.node;
				node.set_position(position);
				node.set_rotation(rotation);
				node.set_scale(scales);
			}
		}
		return i;
	}
}
