#include <egfx/mesh_definition.h>

#include <catch2/catch.hpp>

ot::math::plane const cube_planes[6] = {
	{{0, 0, 1}, 0.5},
	{{1, 0, 0}, 0.5},
	{{0, 1, 0}, 0.5},
	{{-1, 0, 0}, 0.5},
	{{0, -1, 0}, 0.5},
	{{0, 0, -1}, 0.5},
};

TEST_CASE("mesh::make_cube", "[graphics]")
{
	ot::egfx::mesh_definition const cube(cube_planes);
	
	auto const faces = cube.get_faces();
	REQUIRE(faces.size() == 6);

	auto const edges = cube.get_edges();
	REQUIRE(std::distance(edges.begin(), edges.end()) == 12);

	// For all faces, test its edges
	for (ot::egfx::face::cref const face : faces)
	{
		ot::egfx::vertex::id visited_vertices[4];

		REQUIRE(face.get_vertex_count() == 4);
		REQUIRE(is_normalized(face.get_normal()));
		REQUIRE(float_eq(face.get_plane(), cube_planes[static_cast<size_t>(face.get_id())]));

		// Test that each face has 4 edges
		auto const half_edges = face.get_half_edges();
		auto edge_it = half_edges.begin();
		ot::egfx::half_edge::cref current_edge = *edge_it;
		visited_vertices[0] = current_edge.get_target_vertex().get_id();
		REQUIRE(current_edge.get_face() == face);
		ot::egfx::half_edge::cref twin = current_edge.get_twin();
		REQUIRE(twin.get_twin() == current_edge);

		for (size_t j = 1; j < 4; ++j)
		{
			++edge_it;
			current_edge = *edge_it;
			auto const target_vertex = current_edge.get_target_vertex();
			REQUIRE(std::find(visited_vertices, visited_vertices + j, target_vertex.get_id()) == visited_vertices + j);
			visited_vertices[j] = target_vertex.get_id();
			REQUIRE(current_edge.get_face() == face);
			twin = current_edge.get_twin();
			REQUIRE(twin.get_twin() == current_edge);
		}
	}

	auto const vertices = cube.get_vertices();
	REQUIRE(vertices.size() == 8);

	for (ot::egfx::vertex::cref const vertex : vertices)
	{
		auto const half_edges = vertex.get_half_edges();
		
		REQUIRE(std::distance(half_edges.begin(), half_edges.end()) == 3); // three edges around each vertex
		for (ot::egfx::half_edge::cref const he : half_edges)
		{
			REQUIRE(he.get_source_vertex() == vertex);
			REQUIRE(he.get_twin().get_target_vertex() == vertex);
		}
	}

	auto const bounds = cube.get_bounds();
	REQUIRE(float_eq(bounds.max(), ot::math::point3f{ 0.5, 0.5, 0.5 }));
	REQUIRE(float_eq(bounds.min(), ot::math::point3f{ -0.5, -0.5, -0.5 }));
}

TEST_CASE("half_edge::ref::split", "[graphics­]")
{
	ot::egfx::mesh_definition cube(cube_planes);
	auto const top_face_edges = cube.get_faces()[0].get_half_edges();
	
	// Find the edge pointing at the {1, 1, 1} vertex
	auto const found_edge = std::find_if(top_face_edges.begin(), top_face_edges.end(), [&cube](ot::egfx::half_edge::cref const h)
	{
		return float_eq(h.get_target_vertex().get_position(), { 0.5, 0.5, 0.5 });
	});

	REQUIRE(found_edge != top_face_edges.end());
	auto const current_edge = *found_edge;
	auto const current_twin = current_edge.get_twin();
	auto const current_target = current_edge.get_target_vertex();
	auto const current_source = current_edge.get_source_vertex();

	auto const new_edge = current_edge.split_at({ 1, 0.5, 1 });
	auto const new_twin = current_edge.get_twin();

	REQUIRE(current_edge.get_source_vertex() == current_source);
	REQUIRE(new_edge.get_twin() == current_twin);
	REQUIRE(new_edge.get_target_vertex() == current_target);
	REQUIRE(new_twin.get_twin() == current_edge);
	REQUIRE(new_twin.get_target_vertex() == current_source);
	REQUIRE(current_twin.get_twin() == new_edge);
	REQUIRE(current_twin.get_source_vertex() == current_target);

	auto const new_face_edges = cube.get_faces()[0].get_half_edges();
	REQUIRE(std::distance(new_face_edges.begin(), new_face_edges.end()) == 5);
}

TEST_CASE("face::ref::split", "[graphics]")
{
	ot::egfx::mesh_definition cube(cube_planes);
	ot::egfx::face::ref const top_face = cube.get_faces()[0];

	ot::math::plane const cutting_plane{ {1, 0, 0}, 0.f };
	top_face.split(cutting_plane);

	REQUIRE(cube.get_faces().size() == 7);
	REQUIRE(cube.get_half_edges().size() == 30);
	REQUIRE(cube.get_vertices().size() == 10);
}
