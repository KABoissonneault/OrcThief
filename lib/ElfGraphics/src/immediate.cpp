#include "egfx/immediate.h"

#include "mesh_definition.h"

#include <im3d.h>

namespace ot::egfx::im
{
	void draw_wiremesh(mesh_definition const& m, math::transform_matrix const& t, float size, color c)
	{
		Im3d::Context& context = Im3d::GetContext();
		context.begin(Im3d::PrimitiveMode_Lines);
		for (half_edge::cref he : m.get_half_edges())
		{
			context.vertex(transform(he.get_source_vertex().get_position(), t), size, Im3d::Color(c.r, c.g, c.b, c.a));
			context.vertex(transform(he.get_target_vertex().get_position(), t), size, Im3d::Color(c.r, c.g, c.b, c.a));
		}
		context.end();
	}

	void draw_mesh(mesh_definition const& m, math::transform_matrix const& t, color col)
	{
		Im3d::Context& context = Im3d::GetContext();
		context.begin(Im3d::PrimitiveMode_Triangles);

		for_each_triangle(m, [&](vertex::cref a, vertex::cref b, vertex::cref c)
		{
			context.vertex(transform(a.get_position(), t), 1.f, Im3d::Color(col.r, col.g, col.b, col.a));
			context.vertex(transform(b.get_position(), t), 1.f, Im3d::Color(col.r, col.g, col.b, col.a));
			context.vertex(transform(c.get_position(), t), 1.f, Im3d::Color(col.r, col.g, col.b, col.a));
		});

		context.end();
	}

	void draw_face(face::cref face, math::transform_matrix const& t, color col)
	{
		Im3d::Context& context = Im3d::GetContext();
		context.begin(Im3d::PrimitiveMode_Triangles);

		for_each_triangle(face, [&](vertex::cref a, vertex::cref b, vertex::cref c)
		{
			context.vertex(transform(a.get_position(), t), 1.f, Im3d::Color(col.r, col.g, col.b, col.a));
			context.vertex(transform(b.get_position(), t), 1.f, Im3d::Color(col.r, col.g, col.b, col.a));
			context.vertex(transform(c.get_position(), t), 1.f, Im3d::Color(col.r, col.g, col.b, col.a));
		});

		context.end();
	}
}