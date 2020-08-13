#pragma once

#include "mesh_definition.fwd.h"

#include "math/vector3.h"
#include "math/plane.h"
#include "math/aabb.h"
#include "math/ray.h"
#include "math/line.h"
#include "core/size_t.h"
#include "core/iterator/arrow_proxy.h"

#include <vector>
#include <span>
#include <cassert>
#include <optional>

namespace ot::graphics
{	
	// A vertex is a point in 3d space. A vertex belongs to many faces, and has many "ingoing" and "outgoing" half-edges
	namespace vertex
	{
		enum class id : size_t { none = -1 };
	}

	// A half-edge forms a pair with another half-edge to represent a full edge between two vertices in the mesh. Both half-edges are parallel and are conceptually
	// connected between the two same vertices, but are part of two different faces.
	// Each half-edge has a reference to a vertex, its other half-edge (twin), its adjacent face, and the next half-edge along the same face
	// Reference: https://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
	namespace half_edge
	{
		enum class id : size_t { none = -1 };
	}

	// A section of a plane, surrounded by at least 3 vertices. 
	namespace face
	{
		enum class id : size_t { none = -1 };
	}

	namespace detail
	{
		template<typename IdType>
		class id_range;

		template<template<typename Iterator> typename Iteration>
		class half_edge_range;

		template<typename Iterator>
		class face_half_edge_iteration;

		template<typename Iterator>
		class vertex_half_edge_iteration;

		class face_vertex_range;
	}

	namespace vertex
	{
		// Get the position in the mesh's local space of the vertex
		[[nodiscard]] math::point3d get_position(mesh_definition const& m, id v);

		// Returns a range of the half-edges around the given vertex
		[[nodiscard]] auto get_half_edges(mesh_definition const& m, id vertex) -> detail::half_edge_range<detail::vertex_half_edge_iteration>;
	}

	namespace half_edge
	{
		// Returns the vertex the half-edge originates from
		[[nodiscard]] vertex::id get_source_vertex(mesh_definition const& m, id e);

		// Returns the vertex the half-edge is pointing to
		[[nodiscard]] vertex::id get_target_vertex(mesh_definition const& m, id e);

		// Returns the face the half-edge is on
		[[nodiscard]] face::id get_face(mesh_definition const& m, id e);

		// Returns the half-edge's twin
		[[nodiscard]] id get_twin(mesh_definition const& m, id e);

		// Returns the line from the source vertex to the target vertex
		[[nodiscard]] math::line get_line(mesh_definition const& m, id e);

		// Splits the half-edge and its twin in two at the given point, creating two new half-edges 
		// The input edge is modified to have the new edge as its "next", and the new half-edge is returned
		// The new half-edge will have the current twin as its twin, and the current half-edge will have the new twin as its twin
		// 
		//   Before:                                  After:
		//     Current Edge                             Current Edge  New Vertex     New Edge
		//   A --------------------------------> B    A ----------------> C ----------------> B
		//   A <-------------------------------- B    A <---------------- C <---------------- B
		//     Current Twin                             New Twin                 Current Twin
		// 
		// Expects:
		//   - 'point' must be on the edge
		// 
		// Returns: The new edge after the input one
		half_edge::id split_at(mesh_definition & m, id edge_id, math::point3d point);
	}

	namespace face
	{
		// Returns the normal vector of the face
		[[nodiscard]] math::vector3d get_normal(mesh_definition const& m, id face);

		// Returns a range of the half-edges around the given face
		[[nodiscard]] auto get_half_edges(mesh_definition const& m, id face) -> detail::half_edge_range<detail::face_half_edge_iteration>;

		// Returns a range of vertices around the given face
		[[nodiscard]] auto get_vertices(mesh_definition const& m, id face) -> detail::face_vertex_range;

		// Returns the number of vertices on the face
		[[nodiscard]] size_t get_vertex_count(mesh_definition const& m, id face);

		// Returns the plane parallel to the face
		[[nodiscard]] math::plane get_plane(mesh_definition const& m, id face);

		// Returns whether the point is on the face. 
		// The point must be in the local coordinates of the mesh definition
		[[nodiscard]] bool is_on_face(mesh_definition const& m, id face, math::point3d p);
	}

	// A mesh representing a three-dimensional manifold formed of faces, vertices, and pairs of half-edges between each vertex
	// 
	class mesh_definition
	{
		struct vertex_data
		{
			math::point3d position;
			half_edge::id first_edge;
		};
		
		struct half_edge_data
		{
			vertex::id vertex; // vertex at the tip of the half edge
			face::id face; // the face this half-edge borders
			half_edge::id twin; // other half-edge in the pair
			half_edge::id next; // the next half-edge along the face
		};
				
		struct face_data
		{
			half_edge::id first_edge; // arbitrary half-edge along the face
		};

		std::vector<vertex_data> vertices;
		std::vector<half_edge_data> half_edges;
		std::vector<face_data> faces;
		math::aabb bounds{};

		// Accessors
		[[nodiscard]] half_edge_data& get_half_edge(half_edge::id id) { return half_edges[static_cast<size_t>(id)]; }
		[[nodiscard]] half_edge_data const& get_half_edge(half_edge::id id) const { return half_edges[static_cast<size_t>(id)]; }
		[[nodiscard]] vertex_data& get_vertex(vertex::id id) { return vertices[static_cast<size_t>(id)]; }
		[[nodiscard]] vertex_data const& get_vertex(vertex::id id) const { return vertices[static_cast<size_t>(id)]; }
		[[nodiscard]] face_data& get_face(face::id id) { return faces[static_cast<size_t>(id)]; }
		[[nodiscard]] face_data const& get_face(face::id id) const { return faces[static_cast<size_t>(id)]; }

	public:
		[[nodiscard]] detail::id_range<vertex::id> get_vertices() const noexcept;
		[[nodiscard]] detail::id_range<face::id> get_faces() const noexcept;
		[[nodiscard]] detail::id_range<half_edge::id> get_half_edges() const noexcept;
		[[nodiscard]] math::aabb get_bounds() const noexcept { return bounds; }

		// Factories

		// Constructs a mesh from a sequence of planes
		// The mesh will have as many faces as the number of input planes, and the faces will preserve the same order as the plane with the same normal
		[[nodiscard]] static mesh_definition make_from_planes(std::span<const math::plane> planes);

		[[nodiscard]] static mesh_definition const& get_cube();

	private:
		template<template<typename Iterator> typename Iteration>
		friend class detail::half_edge_range;

		template<typename Iterator>
		friend class detail::face_half_edge_iteration;

		template<typename Iterator>
		friend class detail::vertex_half_edge_iteration;

		friend class detail::face_vertex_range;

		friend math::point3d vertex::get_position(mesh_definition const& m, vertex::id v);
		friend auto vertex::get_half_edges(mesh_definition const& m, vertex::id vertex) -> detail::half_edge_range<detail::vertex_half_edge_iteration>;
		friend vertex::id half_edge::get_source_vertex(mesh_definition const& m, half_edge::id e);
		friend vertex::id half_edge::get_target_vertex(mesh_definition const& m, half_edge::id e);
		friend half_edge::id half_edge::get_twin(mesh_definition const& m, half_edge::id v);
		friend face::id half_edge::get_face(mesh_definition const& m, half_edge::id v);
		friend half_edge::id half_edge::split_at(mesh_definition & m, half_edge::id v, math::point3d p);
		friend math::vector3d face::get_normal(mesh_definition const& m, face::id v);
		friend auto face::get_half_edges(mesh_definition const& m, face::id face) -> detail::half_edge_range<detail::face_half_edge_iteration>;
		friend auto face::get_vertices(mesh_definition const& m, face::id face) -> detail::face_vertex_range;
		friend size_t face::get_vertex_count(mesh_definition const& m, face::id face);
		friend math::plane face::get_plane(mesh_definition const& m, face::id face);

		struct point_intersection;
		static std::vector<point_intersection> find_intersections(std::span<const math::plane> planes, std::vector<vertex_data>& vertices, std::vector<half_edge_data>& half_edges);
		static void resolve_edge_directions(std::span<const math::plane> planes, std::span<point_intersection const> intersections, std::span<half_edge_data> half_edges, std::span<face_data> faces);
		static void update_bounds(math::aabb& bounds, std::span<vertex_data const> vertices);
	};

	namespace detail
	{
		template<typename IdType>
		class id_range
		{
			IdType first;
			IdType after_last;
		public:
			using value_type = IdType;

			id_range(IdType first, IdType after_last)
				: first(first)
				, after_last(after_last)
			{

			}

			class iterator
			{
				IdType id;
			public:
				explicit iterator(IdType id)
					: id(id)
				{

				}

				using value_type = IdType;
				using reference = value_type;
				using pointer = value_type const*;
				using difference_type = ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

				reference operator*() const
				{
					return id;
				}
				arrow_proxy<value_type> operator->() const
				{
					return { id };
				}
				iterator& operator++()
				{
					id = value_type(static_cast<size_t>(id) + 1);
					return *this;
				}
				iterator operator++(int)
				{
					auto const copy = *this;
					++(*this);
					return copy;
				}
				friend bool operator==(iterator lhs, iterator rhs) noexcept
				{
					return lhs.id == rhs.id;
				}
				friend bool operator!=(iterator lhs, iterator rhs) noexcept
				{
					return lhs.id != rhs.id;
				}
			};

			[[nodiscard]] iterator begin() const noexcept { return iterator{ first }; }
			[[nodiscard]] iterator end() const noexcept { return iterator{ after_last }; }
			[[nodiscard]] size_t size() const noexcept { return static_cast<size_t>(after_last) - static_cast<size_t>(first); }
			[[nodiscard]] value_type operator[](size_t i) const noexcept { return value_type(i); }
		};

		template<template<typename Iterator> typename Iteration >
		class half_edge_range
		{
			mesh_definition const* m = nullptr;
			half_edge::id first = half_edge_id::empty;

		public:
			using value_type = half_edge::id;

			half_edge_range() = default;
			half_edge_range(mesh_definition const* m, half_edge::id first) noexcept
				: m(m)
				, first(first)
			{

			}

			class iterator : public Iteration<iterator>
			{
				mesh_definition const* m = nullptr;
				half_edge::id first = half_edge_id::none;
				half_edge::id current = half_edge_id::none;

				friend Iteration<iterator>;

			public:
				iterator() = default;
				iterator(mesh_definition const* m, half_edge::id first, half_edge::id current) noexcept
					: m(m)
					, first(first)
					, current(current)
				{

				}

				using value_type = half_edge::id;
				using reference = value_type;
				using pointer = value_type*;
				using difference_type = size_t;
				using iterator_category = std::input_iterator_tag;

				reference operator*() const
				{
					return current;
				}
				arrow_proxy<value_type> operator->() const
				{
					return { current };
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

			[[nodiscard]] iterator begin() const { return { m, first, first }; }
			[[nodiscard]] iterator end() const { return { m, first, half_edge::id::none }; }
		};

		template<typename Iterator>
		class face_half_edge_iteration
		{
		public:
			using derived = Iterator;
			auto operator++() -> derived&
			{
				auto& self = static_cast<derived&>(*this);

				auto const next = self.m->get_half_edge(self.current).next;
				if (next == self.first)
				{
					self.current = half_edge::id::none;
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
		class vertex_half_edge_iteration
		{
		public:
			using derived = Iterator;
			auto operator++() -> derived&
			{
				auto& self = static_cast<derived&>(*this);

				auto const next = self.m->get_half_edge(self.m->get_half_edge(self.current).twin).next;
				if (next == self.first)
				{
					self.current = half_edge::id::none;
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

		class face_vertex_range
		{
			mesh_definition const* m = nullptr;
			half_edge::id first = half_edge::id::none;

		public:
			using value_type = vertex::id;

			face_vertex_range() = default;
			face_vertex_range(mesh_definition const* m, half_edge::id first)
				: m(m)
				, first(first)
			{

			}

			class iterator : public detail::face_half_edge_iteration<iterator>
			{
				mesh_definition const* m = nullptr;
				half_edge::id first = half_edge::id::none;
				half_edge::id current = half_edge::id::none;

				friend face_half_edge_iteration<iterator>;

			public:
				iterator() = default;
				iterator(mesh_definition const* m, half_edge::id first, half_edge::id current) noexcept
					: m(m)
					, first(first)
					, current(current)
				{

				}

				using value_type = vertex::id;
				using reference = value_type;
				using pointer = value_type*;
				using difference_type = size_t;
				using iterator_category = std::forward_iterator_tag;

				reference operator*() const
				{
					return get_source_vertex(*m, current);
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

			[[nodiscard]] iterator begin() const noexcept { return { m, first, first }; }
			[[nodiscard]] iterator end() const noexcept { return { m, first, half_edge::id::none }; }
		};
	}

	inline detail::id_range<vertex::id> mesh_definition::get_vertices() const noexcept
	{
		return { vertex::id(0), vertex::id(vertices.size()) };
	}
	inline detail::id_range<face::id> mesh_definition::get_faces() const noexcept
	{
		return { face::id(0), face::id(faces.size()) };
	}
	inline detail::id_range<half_edge::id> mesh_definition::get_half_edges() const noexcept
	{
		return { half_edge::id(0), half_edge::id(half_edges.size()) };
	}

	namespace vertex
	{
		inline math::point3d get_position(mesh_definition const& m, vertex::id vertex)
		{
			return m.get_vertex(vertex).position;
		}

		inline auto get_half_edges(mesh_definition const& m, vertex::id vertex) -> detail::half_edge_range<detail::vertex_half_edge_iteration>
		{
			return { &m, m.get_vertex(vertex).first_edge };
		}
	}

	namespace half_edge
	{
		inline vertex::id get_source_vertex(mesh_definition const& m, id e)
		{
			return m.get_half_edge(m.get_half_edge(e).twin).vertex;
		}

		inline vertex::id get_target_vertex(mesh_definition const& m, id e)
		{
			return m.get_half_edge(e).vertex;
		}

		inline id get_twin(mesh_definition const& m, id e)
		{
			return m.get_half_edge(e).twin;
		}

		inline face::id get_face(mesh_definition const& m, id e)
		{
			return m.get_half_edge(e).face;
		}

		inline math::line get_line(mesh_definition const& m, id e)
		{
			return { get_position(m, get_source_vertex(m, e)), get_position(m, get_target_vertex(m, e)) };
		}
	}

	namespace face
	{
		inline auto get_half_edges(mesh_definition const& m, id face) -> detail::half_edge_range<detail::face_half_edge_iteration>
		{
			return { &m, m.get_face(face).first_edge };
		}

		inline auto get_vertices(mesh_definition const& m, face::id face) -> detail::face_vertex_range
		{
			return { &m, m.get_face(face).first_edge };
		}

		inline size_t get_vertex_count(mesh_definition const& m, face::id face)
		{
			auto const he = get_half_edges(m, face);
			return std::distance(he.begin(), he.end());
		}
	}
}