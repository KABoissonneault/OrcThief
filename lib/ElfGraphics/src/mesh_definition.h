#pragma once

#include "egfx/mesh_definition.h"

namespace ot::egfx
{
	template<typename Function>
	void for_each_triangle(face::cref face, Function&& f);

	template<typename Function>
	inline void for_each_triangle(mesh_definition const& m, Function&& f)
	{
		for (face::cref const face : m.get_faces())
		{
			for_each_triangle(face, f);
		}
	}

	template<typename Function>
	inline void for_each_triangle(face::cref face, Function&& f)
	{
		auto const vertices = face.get_vertices();
		auto vertex_it = vertices.begin();

		vertex::cref const first_vertex = *vertex_it++;
		vertex::cref second_vertex = *vertex_it++;
		vertex::cref third_vertex = *vertex_it++;

		f(first_vertex, second_vertex, third_vertex);

		while (vertex_it != vertices.end())
		{
			second_vertex = third_vertex;
			third_vertex = *vertex_it++;
			f(first_vertex, second_vertex, third_vertex);
		}
	}
}
