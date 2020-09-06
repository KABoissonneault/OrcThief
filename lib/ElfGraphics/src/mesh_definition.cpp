#include "mesh_definition.h"

#include <numeric>

namespace ot::egfx
{	
	namespace half_edge
	{
		//   Before:                                  After:
		//     Current Edge                             Current Edge  New Vertex     New Edge
		//   A --------------------------------> B    A ----------------> C ----------------> B
		//   A <-------------------------------- B    A <---------------- C <---------------- B
		//     Current Twin                             New Twin                 Current Twin
		auto ref::split_at(math::point3d point) const -> ref
		{
			m->half_edges.reserve(m->half_edges.size() + 2);

			auto const edge_id = e;

			mesh_definition::half_edge_data& edge = m->get_half_edge_data(edge_id);

			auto const twin_id = edge.twin;
			mesh_definition::half_edge_data& twin = m->get_half_edge_data(twin_id);

			auto const new_edge_id = id(m->half_edges.size());
			auto& new_edge = m->half_edges.emplace_back();

			auto const new_twin_id = id(m->half_edges.size());
			auto& new_twin = m->half_edges.emplace_back();

			auto const new_vertex_id = vertex::id(m->vertices.size());
			mesh_definition::vertex_data& new_vertex = m->vertices.emplace_back();
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

			return { *m, new_edge_id };
		}
	}

	namespace face
	{
		math::vector3d cref::get_normal() const
		{
			return m->get_face_data(f).normal;
		}

		math::plane cref::get_plane() const
		{
			mesh_definition::face_data const& data = m->get_face_data(f);
			mesh_definition::half_edge_data const& h = m->get_half_edge_data(data.first_edge);
			math::point3d const p = vertex::cref{ *m, h.vertex }.get_position();
			math::vector3d const n = get_normal();
			double const d = dot_product(vector_from_origin(p), n);
			return { n, d };
		}

		bool cref::is_on_face(math::point3d p) const
		{
			math::vector3d const face_normal = get_normal();

			for (half_edge::cref const he : get_half_edges())
			{
				math::line const l = he.get_line();

				math::vector3d const v1 = l.b - l.a;
				math::vector3d const v2 = l.a - p;

				math::vector3d const vr = cross_product(v1, v2);

				// If the cross product is in the same direction as the face normal, the point is outside the face
				if (dot_product(vr, face_normal) > 0)
				{
					return false;
				}
			}

			return true;
		}
	}

	namespace
	{
		math::vector3d get_average_normal(std::span<math::plane const> planes)
		{
			return normalized(std::accumulate(planes.begin(), planes.end(), math::vector3d{}, [](math::vector3d value, math::plane plane) { return value + plane.normal; }));
		}

		struct edge_intersection
		{
			half_edge::id edge;
			face::id planes[2];
		};
	}

	struct mesh_definition::point_intersection
	{
		std::vector<edge_intersection> edges;
		std::vector<face::id> planes;
		vertex::id vertex;
	};

	auto mesh_definition::find_intersections(std::span<const math::plane> planes, std::vector<mesh_definition::vertex_data>& vertices, std::vector<mesh_definition::half_edge_data>& half_edges) -> std::vector<point_intersection>
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
			math::plane const plane1 = planes[i];
			for (size_t j = i + 1; j < planes.size() - 1; ++j)
			{
				math::plane const plane2 = planes[j];
				for (size_t k = j + 1; k < planes.size(); ++k)
				{
					math::plane const plane3 = planes[k];

					auto const intersection_result = find_intersection(plane1, plane2, plane3);
					if (!intersection_result)
					{
						continue;
					}

					std::vector<face::id> intersecting_planes{ face::id(i), face::id(j), face::id(k) };
					std::vector<math::plane> plane_values{ plane1, plane2, plane3 };

					// check for extra planes
					for (size_t l = 0; l < planes.size(); ++l)
					{
						if (l == i || l == j || l == k) continue;

						math::plane const plane4 = planes[l];

						math::plane_side_result const result = get_plane_side(plane4, *intersection_result);
						if (result == math::plane_side_result::on_plane)
						{
							if (l < k)
								goto skip_intersection; // we've already found this intersection

							intersecting_planes.push_back(face::id(l));
							plane_values.push_back(plane4);
						} else if (result == math::plane_side_result::outside)
						{
							goto skip_intersection; // intersection outside the mesh
						}
					}

					{
						auto const vertex_id = vertex::id(vertices.size());
						mesh_definition::vertex_data& vertex = vertices.emplace_back();
						vertex.position = *intersection_result;
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

								mesh_definition::vertex_data& previous_vertex = vertices[static_cast<size_t>(previous_intersection.vertex)];
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

	void mesh_definition::resolve_edge_directions(std::span<const math::plane> planes, std::span<point_intersection const> intersections, std::span<mesh_definition::half_edge_data> half_edges, std::span<mesh_definition::face_data> faces)
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
					if (edge1.planes[0] == edge2.planes[0]) {
						plane_index1 = 0; 
						plane_index2 = 0;
					} else if (edge1.planes[0] == edge2.planes[1]) {
						plane_index1 = 0; 
						plane_index2 = 1;
					} else if (edge1.planes[1] == edge2.planes[0]) {
						plane_index1 = 1; 
						plane_index2 = 0;
					} else if (edge1.planes[1] == edge2.planes[1]){
						plane_index1 = 1; 
						plane_index2 = 1;
					} else {
						continue;
					}

					face::id const shared_plane_id = edge1.planes[plane_index1];
					face::id const other_plane1 = edge1.planes[1 - plane_index1];
					face::id const other_plane2 = edge2.planes[1 - plane_index2];
					math::plane const shared_plane = planes[static_cast<size_t>(shared_plane_id)];
					math::plane const edge1_plane = planes[static_cast<size_t>(other_plane1)];
					math::plane const edge2_plane = planes[static_cast<size_t>(other_plane2)];

					math::vector3d const direction = cross_product(shared_plane.normal, edge1_plane.normal);

					// Determine which edge is "ingoing" and which one is "outgoing"
					auto const [ingoing, outgoing, outgoing_id] = [&]
					{
						mesh_definition::half_edge_data& he1 = half_edges[static_cast<size_t>(edge1.edge)];
						mesh_definition::half_edge_data& he2 = half_edges[static_cast<size_t>(edge2.edge)];
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

	void mesh_definition::update_bounds(math::aabb& bounds, std::span<mesh_definition::vertex_data const> vertices)
	{
		for (mesh_definition::vertex_data const& vertex : vertices)
		{
			bounds.merge(vertex.position);
		}
	}

	mesh_definition mesh_definition::make_from_planes(std::span<const math::plane> planes)
	{
		mesh_definition m;		
		m.faces.resize(planes.size());
		for (size_t i = 0; i < planes.size(); ++i)
			m.faces[i].normal = planes[i].normal;

		std::vector<point_intersection> const intersections = find_intersections(planes, m.vertices, m.half_edges);
		resolve_edge_directions(planes, intersections, m.half_edges, m.faces);
		update_bounds(m.bounds, m.vertices);

		return m;
	}

	mesh_definition const& mesh_definition::get_cube()
	{
		using planes = ot::math::plane[6];
		static mesh_definition const cube = make_from_planes(planes{
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		});
		return cube;
	}
}