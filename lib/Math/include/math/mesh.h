#pragma once

#include "math/vector3.h"
#include "math/plane.h"
#include "core/size_t.h"

#include <vector>
#include <span>

namespace ot::math
{
	class mesh
	{
	public:
		using vertex = point3d;
		enum class vertex_id : size_t { empty = size_t(-1) };
		enum class half_edge_id : size_t { empty = size_t(-1) };
		enum class face_id : size_t { empty = size_t(-1) };

		// A half-edge forms a pair with another half-edge to represent a full edge between two vertices in a mesh. Both half-edges are parallel and are conceptually
		// connected between the two same vertices, but are part of two different faces.
		// Each half-edge has a reference to a single vertex, its other half-edge (pair), its adjacent face, and the next half-edge along the face
		// Reference: https://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
		struct half_edge
		{
			vertex_id vertex; // vertex at the tip of the half edge
			face_id face; // the face this half-edge borders
			half_edge_id twin; // other half-edge in the pair
			half_edge_id next; // the next half-edge along the face
		};

		struct face
		{
			half_edge_id first_edge;
		};

		// Accessors
		[[nodiscard]] half_edge& get_half_edge(half_edge_id id) { return half_edges[static_cast<size_t>(id)]; }
		[[nodiscard]] half_edge const& get_half_edge(half_edge_id id) const { return half_edges[static_cast<size_t>(id)]; }
		[[nodiscard]] vertex& get_vertex(vertex_id id) { return vertices[static_cast<size_t>(id)]; }
		[[nodiscard]] vertex const& get_vertex(vertex_id id) const { return vertices[static_cast<size_t>(id)]; }
		[[nodiscard]] face& get_face(face_id id) { return faces[static_cast<size_t>(id)]; }
		[[nodiscard]] face const& get_face(face_id id) const { return faces[static_cast<size_t>(id)]; }

		[[nodiscard]] std::span<vertex> get_vertices() noexcept { return vertices; }
		[[nodiscard]] std::span<vertex const> get_vertices() const noexcept { return vertices; }
		[[nodiscard]] std::span<face> get_faces() noexcept { return faces; }
		[[nodiscard]] std::span<face const> get_faces() const noexcept { return faces; }

		// Utilities
		[[nodiscard]] half_edge_id get_half_edge_id(half_edge const& e) const { return get_half_edge(e.twin).twin; }

		// Mutators

		// Splits the half-edge and its twin in two at the given point, creating two new half-edges 
		// The input edge is modified to have the new edge as its "next", and the new half-edge is returned
		// The new half-edge will have the current twin as its twin, and the current half-edge will have the new twin as its twin
		// 
		//   Before:                                  After:
		//     Current Edge                             Current Edge     New Edge
		//   A --------------------------------> B    A ---------------->----------------> B
		//   A <-------------------------------- B    A <---------------<----------------- B
		//     Current Twin                             New Twin         Current Twin
		half_edge& split_edge(half_edge_id edge_id, point3d point);
		half_edge& split_edge(half_edge& edge_id, point3d point);

		// Factories

		// Constructs a mesh from a sequence of planes
		// The mesh will have as many faces as the number of input planes, and the faces will preserve the same order as the plane with the same normal
		[[nodiscard]] static mesh make_from_planes(std::span<const plane> planes);

		class face_half_edges
		{
			mesh* m = nullptr;
			half_edge_id first = half_edge_id::empty;

		public:
			face_half_edges() = default;
			face_half_edges(mesh* m, half_edge_id first)
				: m(m)
				, first(first)
			{

			}

			class iterator
			{
				mesh* m = nullptr;
				half_edge_id first = half_edge_id::empty;
				half_edge_id current = half_edge_id::empty;

			public:
				iterator() = default;
				iterator(mesh* m, half_edge_id first, half_edge_id current)
					: m(m)
					, first(first)
					, current(current)
				{

				}

				using value_type = half_edge;
				using reference = half_edge&;
				using pointer = half_edge*;
				using difference_type = size_t;
				using iterator_category = std::forward_iterator_tag;

				reference operator*() const
				{
					return m->get_half_edge(current);
				}
				pointer operator->() const
				{
					return &m->get_half_edge(current);
				}
				iterator& operator++()
				{
					if ((*this)->next == first)
					{
						current = half_edge_id::empty;
					}
					else
					{
						current = (*this)->next;
					}
					return *this;
				}
				bool operator==(iterator other) const noexcept
				{
					return m == other.m
						&& first == other.first
						&& current == other.current;
				}
				bool operator!=(iterator other) const noexcept
				{
					return !(*this == other);
				}
			};

			iterator begin() const { return { m, first, first }; }
			iterator end() const { return { m, first, half_edge_id::empty }; }
		};

		// Ranges
		[[nodiscard]] face_half_edges get_face_half_edges(face_id face)
		{
			return { this, get_face(face).first_edge };
		}

	private:
		std::vector<vertex> vertices;
		std::vector<half_edge> half_edges;
		std::vector<face> faces;
	};
}