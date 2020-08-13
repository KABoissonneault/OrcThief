#pragma once

#include "graphics/mesh_definition.h"

namespace ot::graphics
{
	template<typename Function>
	void for_each_triangle(mesh_definition const& m, Function&& f)
	{
		for (face::id const face : m.get_faces())
		{
			for_each_triangle(m, face, f);
		}
	}

	template<typename Function>
	void for_each_triangle(mesh_definition const& m, face::id face, Function&& f)
	{
		auto const vertices = get_vertices(m, face);
		auto vertex_it = vertices.begin();

		vertex::id const first_vertex = *vertex_it++;
		vertex::id second_vertex = *vertex_it++;
		vertex::id third_vertex = *vertex_it++;

		f(m, first_vertex, second_vertex, third_vertex);

		while (vertex_it != vertices.end())
		{
			second_vertex = third_vertex;
			third_vertex = *vertex_it++;
			f(m, first_vertex, second_vertex, third_vertex);
		}
	}
}