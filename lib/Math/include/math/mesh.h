#pragma once

#include "math/vector3.h"
#include "math/plane.h"
#include "math/AABB.h"
#include "core/size_t.h"
#include "core/iterator/arrow_proxy.h"

#include <vector>
#include <span>
#include <cassert>

namespace ot::math
{
	// A mesh representing a three-dimensional manifold formed of faces, vertices, and pairs of half-edges between each vertex
	// 
	class mesh
	{
	public:
		enum class vertex_id : size_t { empty = size_t(-1) };
		enum class half_edge_id : size_t { empty = size_t(-1) };
		enum class face_id : size_t { empty = size_t(-1) };

		// A point in 3d space. A vertex belongs to many faces, and has many "ingoing" and "outgoing" half-edges
		struct vertex
		{
			point3d position;
			vector3d normal;
			half_edge_id first_edge;
		};

		// A half-edge forms a pair with another half-edge to represent a full edge between two vertices in the mesh. Both half-edges are parallel and are conceptually
		// connected between the two same vertices, but are part of two different faces.
		// Each half-edge has a reference to a vertex, its other half-edge (twin), its adjacent face, and the next half-edge along the same face
		// Reference: https://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
		struct half_edge
		{
			vertex_id vertex; // vertex at the tip of the half edge
			face_id face; // the face this half-edge borders
			half_edge_id twin; // other half-edge in the pair
			half_edge_id next; // the next half-edge along the face
		};

		// A section of a plane, surrounded by at least 3 vertices. 
		struct face
		{
			half_edge_id first_edge; // arbitrary half-edge along the face
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

		/// Utilities
		// Get the id of the half-edge from a reference to the half-edge
		// Precondition: 'e' must be an lvalue reference to a half-edge inside the mesh
		[[nodiscard]] half_edge_id get_half_edge_id(half_edge const& e) const { return get_half_edge(e.twin).twin; }
		// Returns the vertex the half-edge originates from
		[[nodiscard]] vertex_id get_source_vertex(half_edge const& e) const { return get_half_edge(e.twin).vertex; }
		[[nodiscard]] vertex_id get_source_vertex(half_edge_id e) const { return get_source_vertex(get_half_edge(e)); }

		// Mutators

		// Splits the half-edge and its twin in two at the given point, creating two new half-edges 
		// The input edge is modified to have the new edge as its "next", and the new half-edge is returned
		// The new half-edge will have the current twin as its twin, and the current half-edge will have the new twin as its twin
		// 
		//   Before:                                  After:
		//     Current Edge                             Current Edge  New Vertex     New Edge
		//   A --------------------------------> B    A ----------------> C ----------------> B
		//   A <-------------------------------- B    A <---------------- C <---------------- B
		//     Current Twin                             New Twin                 Current Twin
		half_edge& split_edge(half_edge_id edge_id, point3d point);
		half_edge& split_edge(half_edge& edge_id, point3d point);

		// Factories

		// Constructs a mesh from a sequence of planes
		// The mesh will have as many faces as the number of input planes, and the faces will preserve the same order as the plane with the same normal
		[[nodiscard]] static mesh make_from_planes(std::span<const plane> planes);

	private:
		template<typename ValueType, typename Mesh, template<typename Iterator> typename Iteration>
		class half_edge_range;

		template<typename Iterator>
		struct face_half_edges_iteration;

		template<typename Iterator>
		struct vertex_half_edges_iteration;

		class vertex_id_range;

	public:
		
		/// Face functions
		// Returns a range of the half-edges around the given face
		[[nodiscard]] auto get_face_half_edges(face const& face) noexcept -> half_edge_range<half_edge, mesh, face_half_edges_iteration>;

		// Returns a range of the half-edges around the given face
		[[nodiscard]] auto get_face_half_edges(face_id face) noexcept -> half_edge_range<half_edge, mesh, face_half_edges_iteration>;

		// Returns a range of the half-edges around the given face
		[[nodiscard]] auto get_face_half_edges(face const& face) const noexcept -> half_edge_range<half_edge const, mesh const, face_half_edges_iteration>;

		// Returns a range of the half-edges around the given face
		[[nodiscard]] auto get_face_half_edges(face_id face) const noexcept -> half_edge_range<half_edge const, mesh const, face_half_edges_iteration>;

		// Returns a range of vertices around the given face
		[[nodiscard]] auto get_face_vertex_ids(face const& face) const noexcept -> vertex_id_range;

		// Returns a range of vertices around the given face
		[[nodiscard]] auto get_face_vertex_ids(face_id face) const noexcept -> vertex_id_range;

		// Returns the number of vertices on the face
		[[nodiscard]] size_t get_face_vertex_count(face const& face) const noexcept;

		// Returns the number of vertices on the face
		[[nodiscard]] size_t get_face_vertex_count(face_id face) const noexcept
		{
			return get_face_vertex_count(get_face(face));
		}

		// Returns a range of the half-edges around the given vertex
		[[nodiscard]] auto get_vertex_half_edges(vertex_id vertex) noexcept -> half_edge_range<half_edge, mesh, vertex_half_edges_iteration>;

		// Returns a range of the half-edges around the given vertex
		[[nodiscard]] auto get_vertex_half_edges(vertex_id vertex) const noexcept -> half_edge_range<half_edge const, mesh const, vertex_half_edges_iteration>;

		[[nodiscard]] aabb get_bounds() const noexcept { return bounds; }

	private:
		std::vector<vertex> vertices;
		std::vector<half_edge> half_edges;
		std::vector<face> faces;
		aabb bounds{};
	};

	template<typename ValueType, typename Mesh, template<typename Iterator> typename Iteration >
	class mesh::half_edge_range
	{
		Mesh* m = nullptr;
		half_edge_id first = half_edge_id::empty;

	public:
		using value_type = ValueType;

		half_edge_range() = default;
		half_edge_range(Mesh* m, half_edge_id first) noexcept
			: m(m)
			, first(first)
		{

		}

		class iterator : public Iteration<iterator>
		{
			Mesh* m = nullptr;
			half_edge_id first = half_edge_id::empty;
			half_edge_id current = half_edge_id::empty;

			friend Iteration<iterator>;

		public:
			iterator() = default;
			iterator(Mesh* m, half_edge_id first, half_edge_id current) noexcept
				: m(m)
				, first(first)
				, current(current)
			{

			}

			using value_type = ValueType;
			using reference = value_type&;
			using pointer = value_type*;
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

	template<typename Iterator>
	struct mesh::face_half_edges_iteration
	{
		using derived = Iterator;
		auto operator++() -> derived&
		{
			auto& self = static_cast<derived&>(*this);

			auto const next = self.m->get_half_edge(self.current).next;
			if (next == self.first)
			{
				self.current = half_edge_id::empty;
			} else
			{
				self.current = next;
			}

			return self;
		}

		auto operator++(int) -> derived
		{
			auto copy = static_cast<derived&>(*this);
			++(*this);
			return copy;
		}
	};

	template<typename Iterator>
	struct mesh::vertex_half_edges_iteration
	{
		using derived = Iterator;
		auto operator++() -> derived&
		{
			auto& self = static_cast<derived&>(*this);

			auto const next = self.m->get_half_edge(self->twin).next;
			if (next == self.first)
			{
				self.current = half_edge_id::empty;
			} else
			{
				self.current = next;
			}

			return self;
		}

		auto operator++(int) -> derived
		{
			auto copy = static_cast<derived&>(*this);
			++(*this);
			return copy;
		}
	};

	inline auto mesh::get_face_half_edges(face const& face) noexcept -> half_edge_range<half_edge, mesh, face_half_edges_iteration>
	{
		return { this, face.first_edge };
	}
	inline auto mesh::get_face_half_edges(face_id face) noexcept -> half_edge_range<half_edge, mesh, face_half_edges_iteration>
	{
		return get_face_half_edges(get_face(face));
	}
	inline auto mesh::get_face_half_edges(face const& face) const noexcept -> half_edge_range<half_edge const, mesh const, face_half_edges_iteration>
	{
		return { this, face.first_edge };
	}
	inline auto mesh::get_face_half_edges(face_id face) const noexcept -> half_edge_range<half_edge const, mesh const, face_half_edges_iteration>
	{
		return get_face_half_edges(get_face(face));
	}
	inline auto mesh::get_vertex_half_edges(vertex_id vertex) noexcept -> half_edge_range<half_edge, mesh, vertex_half_edges_iteration>
	{
		return { this, get_vertex(vertex).first_edge };
	}
	inline auto mesh::get_vertex_half_edges(vertex_id vertex) const noexcept -> half_edge_range<half_edge const, mesh const, vertex_half_edges_iteration>
	{
		return half_edge_range<half_edge const, mesh const, vertex_half_edges_iteration>{ this, get_vertex(vertex).first_edge };
	}
	inline size_t mesh::get_face_vertex_count(face const& face) const noexcept
	{
		auto const he = get_face_half_edges(face);
		return std::distance(he.begin(), he.end());
	}

	class mesh::vertex_id_range
	{
		mesh const* m = nullptr;
		half_edge_id first = half_edge_id::empty;

	public:
		using value_type = vertex_id;

		vertex_id_range() = default;
		vertex_id_range(mesh const* m, half_edge_id first)
			: m(m)
			, first(first)
		{

		}

		class iterator : public face_half_edges_iteration<iterator>
		{
			mesh const* m = nullptr;
			half_edge_id first = half_edge_id::empty;
			half_edge_id current = half_edge_id::empty;

			friend face_half_edges_iteration<iterator>;

		public:
			iterator() = default;
			iterator(mesh const* m, half_edge_id first, half_edge_id current) noexcept
				: m(m)
				, first(first)
				, current(current)
			{

			}

			using value_type = vertex_id;
			using reference = value_type;
			using pointer = value_type*;
			using difference_type = size_t;
			using iterator_category = std::forward_iterator_tag;

			reference operator*() const
			{
				return m->get_source_vertex(current);
			}
			arrow_proxy<value_type> operator->() const
			{
				return arrow_proxy(**this);
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

		iterator begin() const noexcept { return { m, first, first }; }
		iterator end() const noexcept { return { m, first, half_edge_id::empty }; }
	};

	inline auto mesh::get_face_vertex_ids(face const& face) const noexcept -> vertex_id_range
	{
		return { this, face.first_edge };
	}

	inline auto mesh::get_face_vertex_ids(face_id face) const noexcept -> vertex_id_range
	{
		return get_face_vertex_ids(get_face(face));
	}
}