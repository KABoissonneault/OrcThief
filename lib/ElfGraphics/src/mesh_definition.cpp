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
		auto ref::split_at(math::point3f point) const -> ref
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
		detail::face_data const& cref::get_face_data() const noexcept
		{
			return m->get_face_data(f);
		}

		math::vector3f cref::get_normal() const
		{
			return get_face_data().normal;
		}

		math::plane cref::get_plane() const
		{
			detail::face_data const& data = get_face_data();
			detail::half_edge_data const& h = m->get_half_edge_data(data.first_edge);
			math::point3f const p = m->get_vertex(h.vertex).get_position();
			math::vector3f const n = get_normal();
			float const d = dot_product(vector_from_origin(p), n);
			return { n, d };
		}

		bool cref::is_on_face(math::point3f p) const
		{
			math::vector3f const face_normal = get_normal();

			for (half_edge::cref const he : get_half_edges())
			{
				math::line const l = he.get_line();

				math::vector3f const v1 = l.b - l.a;
				math::vector3f const v2 = l.a - p;

				math::vector3f const vr = cross_product(v1, v2);

				// If the cross product is in the same direction as the face normal, the point is outside the face
				if (dot_product(vr, face_normal) > 0)
				{
					return false;
				}
			}

			return true;
		}

		half_edge::cref cref::get_first_half_edge() const
		{
			return m->get_half_edge(get_face_data().first_edge);
		}

		detail::face_data& ref::get_face_data() const noexcept
		{
			return m->get_face_data(f);
		}

		half_edge::ref ref::get_first_half_edge() const
		{
			return m->get_half_edge(get_face_data().first_edge);
		}

		expected<ref, split_fail> ref::split(math::plane const p) const
		{
			struct point_data
			{
				half_edge::ref half_edge; // half-edge before the point
				math::point3f position; // position of the target vertex
				float distance; // distance from plane
				math::plane_side_result side; // side of the plane
			};

			auto make_data = [&p] (half_edge::ref half_edge) -> point_data
			{
				math::point3f const position = half_edge.get_target_vertex().get_position();
				float const distance = p.distance_to(position);
				return {
					half_edge
					, position
					, distance
					, math::distance_to_plane_side(distance)
				};
			};

			auto make_next = [&p, make_data](point_data const& d)
			{
				return make_data(d.half_edge.get_next());
			};

			point_data first = make_data(get_first_half_edge());
			point_data second = make_next(first);
			point_data third = make_next(second);
			half_edge::cref const last = third.half_edge;
			half_edge::id enter_edge_id = half_edge::id::none;
			half_edge::id exit_edge_id = half_edge::id::none;

			do
			{
				if (first.side != second.side) // check if first to second crossed the plane
				{
					if (second.side != math::plane_side_result::on_plane) // if second is inside or outside
					{
						if (first.side != math::plane_side_result::on_plane) // if first is also inside or outside (and opposite of current)
						{
							// we need a new vertex at the actual intersection
							math::point3f const new_vertex = math::find_distance_ray_intersection(first.position, first.distance, second.position, second.distance);
							second.half_edge.split_at(new_vertex);

							if (first.side == math::plane_side_result::inside) // if first is inside and second outside
							{
								// Then second exited the plane
								exit_edge_id = second.half_edge.get_id();
							}
							else // if first is outside and second inside
							{
								// Then second entered the plane
								enter_edge_id = second.half_edge.get_id();
							}

							// first now refers to second, whose target vertex has been changed to be the new split point on the plane
							first.half_edge = second.half_edge;
							first.position = new_vertex;
							first.distance = 0.f;
							first.side = math::plane_side_result::on_plane;

							if (enter_edge_id != half_edge::id::none && exit_edge_id != half_edge::id::none)
								break;

							second = make_next(first);
							third = make_next(second);
						}
					}
					else // if second is on plane
					{
						if (
							first.side == math::plane_side_result::on_plane // if first is on plane
							|| third.side == math::plane_side_result::on_plane // or if third is on plane
							|| first.side == third.side // or if both are on the same side
							)
						{
							if (first.side == math::plane_side_result::inside
								|| third.side == math::plane_side_result::inside)
							{
								// Then we have either an edge on the plane leading inside,
								// an inside edge leading on the plane
								// or two inside edges
								// The face is inside the plane

								first.side = math::plane_side_result::inside;
								enter_edge_id = exit_edge_id = half_edge::id::none;
								break;
							}
							else if(first.side == math::plane_side_result::outside
								|| third.side == math::plane_side_result::outside)
							{
								// Then we have either an edge on the plane leading outside,
								// an outside edge leading on the plane
								// or two outside edges
								// The face is outside the plane

								first.side = math::plane_side_result::outside;
								enter_edge_id = exit_edge_id = half_edge::id::none;
								break;
							}
						}
						else // if first is inside or outside, and second is on the plane, and third is inside or outside (but opposite to first)
						{
							if (first.side == math::plane_side_result::inside)
							{
								// Then second is exiting the plane
								exit_edge_id = second.half_edge.get_id();
							}
							else // first is outside
							{
								// Then second is entering the plane
								enter_edge_id = second.half_edge.get_id();
							}

							if (enter_edge_id != half_edge::id::none && exit_edge_id != half_edge::id::none)
								break;
						}
					}
				}

				first = second;
				second = third;
				third = make_next(third);
			} while (third.half_edge != last);

			assert((enter_edge_id != half_edge::id::none) == (exit_edge_id != half_edge::id::none) && "Partial edge pair found - logic error");

			if (enter_edge_id != half_edge::id::none && exit_edge_id != half_edge::id::none)
			{
				// The enter edge goes from outside the plane to inside
				// The exit edge goes from inside to outside

				// Insert a new face outside the plane, and a new half-edge pair for the new edge between the two faces
				// We keep the current face as the "inside" face

				face::id const new_face_id{ m->faces.size() };
				detail::face_data& new_face = m->faces.emplace_back();

				m->half_edges.reserve(m->half_edges.size() + 2);
				half_edge::id const outside_edge_id{ m->half_edges.size() };
				detail::half_edge_data& outside_edge = m->half_edges.emplace_back();

				half_edge::id const inside_edge_id{ m->half_edges.size() };
				detail::half_edge_data& inside_edge = m->half_edges.emplace_back();

				detail::half_edge_data& enter_edge = m->get_half_edge_data(enter_edge_id);
				detail::half_edge_data& exit_edge = m->get_half_edge_data(exit_edge_id);

				// Initialize the new half-edges
				outside_edge.twin = inside_edge_id;
				inside_edge.twin = outside_edge_id;

				outside_edge.face = new_face_id;
				inside_edge.face = get_id();

				outside_edge.vertex = exit_edge.vertex;
				inside_edge.vertex = enter_edge.vertex;

				outside_edge.next = exit_edge.next;
				inside_edge.next = enter_edge.next;

				exit_edge.next = inside_edge_id;
				enter_edge.next = outside_edge_id;

				new_face.first_edge = outside_edge_id;
				new_face.normal = get_normal(); // new face has same normal

				return face::ref{ *m, new_face_id };
			}
			else
			{
				switch (first.side)
				{
				case math::plane_side_result::inside: return make_unexpected(split_fail::inside);
				case math::plane_side_result::outside: return make_unexpected(split_fail::outside);
				}

				if (float_cmp(dot_product(get_normal(), p.normal), 0.f) > 0)
					return make_unexpected(split_fail::aligned);
				else
					return make_unexpected(split_fail::opposite_aligned);
			}
		} 
	}

	namespace
	{
		math::vector3f get_average_normal(std::span<math::plane const> planes)
		{
			return normalized(std::accumulate(planes.begin(), planes.end(), math::vector3f{}, [](math::vector3f value, math::plane plane) { return value + plane.normal; }));
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
						} 
						else if (result == math::plane_side_result::outside)
						{
							goto skip_intersection; // intersection outside the mesh
						}
					}

					// Create the new point intersection, and find the new edge intersections
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
					} else if (edge1.planes[1] == edge2.planes[1]) {
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

					math::vector3f const direction = cross_product(shared_plane.normal, edge1_plane.normal);

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

	mesh_definition::mesh_definition(std::span<const math::plane> planes)
	{
		faces.resize(planes.size());
		for (size_t i = 0; i < planes.size(); ++i)
			faces[i].normal = planes[i].normal;

		std::vector<point_intersection> const intersections = find_intersections(planes, vertices, half_edges);
		resolve_edge_directions(planes, intersections, half_edges, faces);
		update_bounds(bounds, vertices);
	}

	mesh_definition const& mesh_definition::get_cube()
	{
		using planes = ot::math::plane[6];
		static mesh_definition const cube{
			planes{
				{{0, 0, 1}, 0.5},
				{{1, 0, 0}, 0.5},
				{{0, 1, 0}, 0.5},
				{{-1, 0, 0}, 0.5},
				{{0, -1, 0}, 0.5},
				{{0, 0, -1}, 0.5},
			}
		};

		return cube;
	}
}