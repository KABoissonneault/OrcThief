#include "math/mesh.h"

#include <numeric>

namespace ot::math
{	
	namespace half_edge
	{
		auto split_at(mesh & m, id edge_id, point3d point) -> id
		{
			m.half_edges.reserve(m.half_edges.capacity() + 2);

			mesh::half_edge_data& edge = m.get_half_edge(edge_id);

			auto const twin_id = edge.twin;
			mesh::half_edge_data& twin = m.get_half_edge(twin_id);

			auto const new_edge_id = id(m.half_edges.size());
			auto& new_edge = m.half_edges.emplace_back();

			auto const new_twin_id = id(m.half_edges.size());
			auto& new_twin = m.half_edges.emplace_back();

			auto const new_vertex_id = vertex::id(m.vertices.size());
			mesh::vertex_data& new_vertex = m.vertices.emplace_back();
			new_vertex.position = point;
			new_vertex.first_edge = new_edge_id;

			new_edge.face = edge.face;
			new_edge.vertex = edge.vertex;
			new_edge.next = edge.next;
			new_edge.twin = twin_id;

			new_twin.face = twin.face;
			new_twin.vertex = twin.vertex;
			new_twin.next = twin.next;
			new_twin.twin = edge_id;

			edge.vertex = new_vertex_id;
			edge.next = new_edge_id;
			edge.twin = new_twin_id;

			twin.vertex = new_vertex_id;
			twin.next = new_twin_id;
			twin.twin = new_edge_id;

			return new_edge_id;
		}
	}

	namespace
	{
		vector3d get_average_normal(std::span<plane const> planes)
		{
			return normalized(std::accumulate(planes.begin(), planes.end(), vector3d{}, [](vector3d value, plane plane) { return value + plane.normal; }));
		}

		struct edge_intersection
		{
			half_edge::id edge;
			face::id planes[2];
		};
	}

	struct mesh::point_intersection
	{
		std::vector<edge_intersection> edges;
		std::vector<face::id> planes;
		vertex::id vertex;
	};

	auto mesh::find_intersections(std::span<const plane> planes, std::vector<mesh::vertex_data>& vertices, std::vector<mesh::half_edge_data>& half_edges) -> std::vector<point_intersection>
	{
		if (planes.size() < 3)
		{
			return {};
		}

		std::vector<point_intersection> intersections;
		intersections.reserve(planes.size() * planes.size());

		// Find the point intersections where 3 or more planes intersect
		for (size_t i = 0; i < planes.size() - 2; ++i)
		{
			plane const plane1 = planes[i];
			for (size_t j = i + 1; j < planes.size() - 1; ++j)
			{
				plane const plane2 = planes[j];
				for (size_t k = j + 1; k < planes.size(); ++k)
				{
					plane const plane3 = planes[k];

					auto const intersection_result = find_intersection(plane1, plane2, plane3);
					if (!intersection_result)
					{
						continue;
					}

					std::vector<face::id> intersecting_planes{ face::id(i), face::id(j), face::id(k) };
					std::vector<plane> plane_values{ plane1, plane2, plane3 };

					// check for extra planes
					for (size_t l = 0; l < planes.size(); ++l)
					{
						if (l == i || l == j || l == k) continue;

						plane const plane4 = planes[l];

						plane_side_result const result = get_plane_side(plane4, *intersection_result);
						if (result == plane_side_result::on_plane)
						{
							if (l < k)
								goto skip_intersection; // we've already found this intersection

							intersecting_planes.push_back(face::id(l));
							plane_values.push_back(plane4);
						} else if (result == plane_side_result::outside)
						{
							goto skip_intersection; // intersection outside the mesh
						}
					}

					{
						auto const vertex_id = vertex::id(vertices.size());
						mesh::vertex_data& vertex = vertices.emplace_back();
						vertex.position = *intersection_result;
						vertex.normal = get_average_normal(plane_values);
						vertex.first_edge = half_edge::id::none;

						point_intersection intersection;
						intersection.vertex = vertex_id;
						intersection.planes = std::move(intersecting_planes);

						// find edge intersections with existing point intersections
						for (point_intersection& previous_intersection : intersections)
						{
							auto const& planes_current = intersection.planes;
							auto const& planes_other = previous_intersection.planes;

							// find 2 planes that are also in the other intersection
							size_t current_found_plane = 0;
							face::id found_planes[2];
							for (face::id const p : planes_current)
							{
								if (std::find(planes_other.begin(), planes_other.end(), p) == planes_other.end())
									continue; // current plane not in the other intersection

								found_planes[current_found_plane] = p;
								++current_found_plane;

								if (current_found_plane == 2)
									break;
							}

							// other intersection does not have enough common planes
							if (current_found_plane < 2)
								continue;

							// create new intersection edges
							half_edges.reserve(half_edges.capacity() + 2);

							auto const he1_id = half_edge::id(half_edges.size());
							auto& he1 = half_edges.emplace_back();

							auto const he2_id = half_edge::id(half_edges.size());
							auto& he2 = half_edges.emplace_back();

							he1.twin = he2_id;
							he1.vertex = previous_intersection.vertex;
							he2.twin = he1_id;
							he2.vertex = intersection.vertex;

							// Resolve the "first edges" of the vertices if needed
							{
								if (vertex.first_edge == half_edge::id::none)
								{
									vertex.first_edge = he1_id;
								}

								mesh::vertex_data& previous_vertex = vertices[static_cast<size_t>(previous_intersection.vertex)];
								if (previous_vertex.first_edge == half_edge::id::none)
								{
									previous_vertex.first_edge = he2_id;
								}
							}

							edge_intersection& ei1 = previous_intersection.edges.emplace_back();
							ei1.edge = he1_id;
							ei1.planes[0] = found_planes[0];
							ei1.planes[1] = found_planes[1];

							edge_intersection& ei2 = intersection.edges.emplace_back();
							ei2.edge = he2_id;
							ei2.planes[0] = found_planes[0];
							ei2.planes[1] = found_planes[1];
						}

						intersections.push_back(std::move(intersection));
					}

				skip_intersection:
					;
				}
			}
		}

		intersections.erase(std::remove_if(intersections.begin(), intersections.end(), [](point_intersection const& p)
			{
				return p.edges.size() <= 2;
			}), intersections.end());

		return intersections;
	}

	void mesh::resolve_edge_directions(std::span<const plane> planes, std::span<point_intersection const> intersections, std::span<mesh::half_edge_data> half_edges, std::span<mesh::face_data> faces)
	{
		// For each vertex, check which half-edges are "ingoing" (ie: pointing at the vertex) or "outgoing" (ie: has its origin at the vertex)
		for (point_intersection const& intersection : intersections)
		{
			auto const& edges = intersection.edges;
			auto const vertex_id = intersection.vertex;

			for (size_t i = 0; i < edges.size() - 1; ++i)
			{
				auto const& edge1 = edges[i];
				for (size_t j = i + 1; j < edges.size(); ++j)
				{
					auto const& edge2 = edges[j];

					// index of the shared plane for each edge intersection
					int plane_index1;
					int plane_index2;
					if (edge1.planes[0] == edge2.planes[0])
					{
						plane_index1 = 0; plane_index2 = 0;
					} else if (edge1.planes[0] == edge2.planes[1])
					{
						plane_index1 = 0; plane_index2 = 1;
					} else if (edge1.planes[1] == edge2.planes[0])
					{
						plane_index1 = 1; plane_index2 = 0;
					} else if (edge1.planes[1] == edge2.planes[1])
					{
						plane_index1 = 1; plane_index2 = 1;
					} else
					{
						continue;
					}

					face::id const shared_plane_id = edge1.planes[plane_index1];
					face::id const other_plane1 = edge1.planes[1 - plane_index1];
					face::id const other_plane2 = edge2.planes[1 - plane_index2];
					plane const shared_plane = planes[static_cast<size_t>(shared_plane_id)];
					plane const edge1_plane = planes[static_cast<size_t>(other_plane1)];
					plane const edge2_plane = planes[static_cast<size_t>(other_plane2)];

					vector3d const direction = cross_product(shared_plane.normal, edge1_plane.normal);

					// Determine which edge is "ingoing" and which one is "outgoing"
					auto const [ingoing, outgoing, outgoing_id] = [&]
					{
						mesh::half_edge_data& he1 = half_edges[static_cast<size_t>(edge1.edge)];
						mesh::half_edge_data& he2 = half_edges[static_cast<size_t>(edge2.edge)];
						if (dot_product(direction, edge2_plane.normal) < 0)
							return std::make_tuple(&he2, &half_edges[static_cast<size_t>(he1.twin)], he1.twin);
						else
							return std::make_tuple(&he1, &half_edges[static_cast<size_t>(he2.twin)], he2.twin);
					}();

					ingoing->next = outgoing_id;
					ingoing->face = shared_plane_id;
					outgoing->face = shared_plane_id;

					faces[static_cast<size_t>(shared_plane_id)].first_edge = outgoing_id;
				}
			}
		}
	}

	void mesh::update_bounds(aabb& bounds, std::span<mesh::vertex_data const> vertices)
	{
		for (mesh::vertex_data const& vertex : vertices)
		{
			bounds.merge(vertex.position);
		}
	}

	mesh mesh::make_from_planes(std::span<const plane> planes)
	{
		mesh m;		
		m.faces.resize(planes.size());

		std::vector<point_intersection> const intersections = find_intersections(planes, m.vertices, m.half_edges);
		resolve_edge_directions(planes, intersections, m.half_edges, m.faces);
		update_bounds(m.bounds, m.vertices);

		return m;
	}
}