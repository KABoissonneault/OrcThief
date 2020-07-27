#include <graphics/mesh_definition.h>

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
	ot::graphics::mesh_definition const cube = ot::graphics::mesh_definition::make_from_planes(cube_planes);
	
	auto const faces = cube.get_faces();
	REQUIRE(faces.size() == 6);

	// For all faces, test its edges
	for (ot::graphics::face::id const face : faces)
	{
		ot::graphics::vertex::id visited_vertices[4];

		REQUIRE(get_vertex_count(cube, face) == 4);
		REQUIRE(is_normalized(get_normal(cube, face)));

		// Test that each face has 4 edges
		auto const half_edges = get_half_edges(cube, face);
		auto edge_it = half_edges.begin();
		ot::graphics::half_edge::id current_edge = *edge_it;
		visited_vertices[0] = get_target_vertex(cube, current_edge);
		REQUIRE(get_face(cube, current_edge) == face);
		ot::graphics::half_edge::id twin = get_twin(cube, current_edge);
		REQUIRE(get_twin(cube, twin) == current_edge);

		for (size_t j = 1; j < 4; ++j)
		{
			++edge_it;
			current_edge = *edge_it;
			auto const target_vertex = get_target_vertex(cube, current_edge);
			REQUIRE(std::find(visited_vertices, visited_vertices + j, target_vertex) == visited_vertices + j);
			visited_vertices[j] = target_vertex;
			REQUIRE(get_face(cube, current_edge) == face);
			twin = get_twin(cube, current_edge);
			REQUIRE(get_twin(cube, twin) == current_edge);
		}
	}

	auto const vertices = cube.get_vertices();
	REQUIRE(vertices.size() == 8);

	for (ot::graphics::vertex::id vertex : vertices)
	{
		auto const half_edges = get_half_edges(cube, vertex);
		
		REQUIRE(std::distance(half_edges.begin(), half_edges.end()) == 3); // three edges around each vertex
		for (ot::graphics::half_edge::id const he : half_edges)
		{
			REQUIRE(get_source_vertex(cube, he) == vertex);
			REQUIRE(get_target_vertex(cube, get_twin(cube, he)) == vertex);
		}
	}

	auto const bounds = cube.get_bounds();
	REQUIRE(float_eq(bounds.max(), ot::math::point3d{ 1., 1., 1. }));
	REQUIRE(float_eq(bounds.min(), ot::math::point3d{ 0., 0., 0. }));
}

TEST_CASE("mesh::split_edge", "[math­]")
{
	ot::graphics::mesh_definition cube = ot::graphics::mesh_definition::make_from_planes(cube_planes);
	auto const top_face_edges = get_half_edges(cube, cube.get_faces()[0]);
	
	// Find the edge pointing at the {1, 1, 1} vertex
	auto const found_edge = std::find_if(top_face_edges.begin(), top_face_edges.end(), [&cube](ot::graphics::half_edge::id const h)
	{
		return float_eq(get_position(cube, get_target_vertex(cube, h)), { 1, 1, 1 });
	});

	REQUIRE(found_edge != top_face_edges.end());
	auto const current_edge = *found_edge;
	auto const current_twin = get_twin(cube, current_edge);
	auto const current_target = get_target_vertex(cube, current_edge);
	auto const current_source = get_source_vertex(cube, current_edge);

	auto const new_edge = split_at(cube, current_edge, { 1, 0.5, 1 });
	auto const new_twin = get_twin(cube, current_edge);

	REQUIRE(get_source_vertex(cube, current_edge) == current_source);
	REQUIRE(get_twin(cube, new_edge) == current_twin);
	REQUIRE(get_target_vertex(cube, new_edge) == current_target);
	REQUIRE(get_twin(cube, new_twin) == current_edge);
	REQUIRE(get_target_vertex(cube, new_twin) == current_source);
	REQUIRE(get_twin(cube, current_twin) == new_edge);
	REQUIRE(get_source_vertex(cube, current_twin) == current_target);

	auto const new_face_edges = get_half_edges(cube, cube.get_faces()[0]);
	REQUIRE(std::distance(new_face_edges.begin(), new_face_edges.end()) == 5);
}
