#include <math/mesh.h>

#include <catch2/catch.hpp>

ot::math::plane const cube_planes[6] = {
	{{0, 0, 1}, 1},
	{{1, 0, 0}, 1},
	{{0, 1, 0}, 1},
	{{-1, 0, 0}, 0},
	{{0, -1, 0}, 0},
	{{0, 0, -1}, 0},
};

TEST_CASE("mesh::make_cube", "[math]")
{
	ot::math::mesh const cube = ot::math::mesh::make_from_planes(cube_planes);
	
	auto const faces = cube.get_faces();
	REQUIRE(faces.size() == 6);

	// For all faces, test its edges
	for (size_t i = 0; i < faces.size(); ++i)
	{
		ot::math::mesh::face const face = faces[i];

		ot::math::mesh::vertex_id visited_vertices[4];

		// Test that each face has 4 edges
		ot::math::mesh::half_edge_id current_id = face.first_edge;
		ot::math::mesh::half_edge current_edge = cube.get_half_edge(face.first_edge);
		ot::math::mesh::half_edge twin = cube.get_half_edge(current_edge.twin);
		visited_vertices[0] = current_edge.vertex;
		REQUIRE(current_edge.face == static_cast<ot::math::mesh::face_id>(i));

		for (size_t j = 1; j < 4; ++j)
		{
			current_id = current_edge.next;
			current_edge = cube.get_half_edge(current_id);
			visited_vertices[j] = current_edge.vertex;
			REQUIRE(current_edge.face == static_cast<ot::math::mesh::face_id>(i));
			twin = cube.get_half_edge(current_edge.twin);
			REQUIRE(twin.twin == current_id);
			REQUIRE(std::find(visited_vertices, visited_vertices + j, current_edge.vertex) == visited_vertices + j);
		}

		REQUIRE(current_edge.next == face.first_edge);
	}

	auto const vertices = cube.get_vertices();
	REQUIRE(vertices.size() == 8);

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		auto const vertex_id = static_cast<ot::math::mesh::vertex_id>(i);
		auto const half_edges = cube.get_vertex_half_edges(vertex_id);

		REQUIRE(std::distance(half_edges.begin(), half_edges.end()) == 3);
		for (ot::math::mesh::half_edge const& he : half_edges)
		{
			REQUIRE(cube.get_half_edge(he.twin).vertex == vertex_id);
		}
	}
}

TEST_CASE("mesh::split_edge", "[math­]")
{
	ot::math::mesh cube = ot::math::mesh::make_from_planes(cube_planes);
	auto const top_face_edges = cube.get_face_half_edges(ot::math::mesh::face_id(0));
	
	// Find the edge pointing at the {1, 1, 1} vertex
	auto const found_edge = std::find_if(top_face_edges.begin(), top_face_edges.end(), [&cube](ot::math::mesh::half_edge const& h)
	{
		return almost_equal(cube.get_vertex(h.vertex).position, { 1, 1, 1 });
	});

	REQUIRE(found_edge != top_face_edges.end());
	
	cube.split_edge(*found_edge, { 1, 0.5, 1 });

	auto const new_face_edges = cube.get_face_half_edges(ot::math::mesh::face_id(0));
	REQUIRE(std::distance(new_face_edges.begin(), new_face_edges.end()) == 5);
}
