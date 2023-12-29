#pragma once

#include "mesh_definition.fwd.h"

#include "math/vector3.h"
#include "math/vector2.h"
#include "math/plane.h"
#include "math/aabb.h"
#include "math/line.h"
#include "core/size_t.h"
#include "core/iterator/arrow_proxy.h"
#include "core/expected.h"

#include <vector>
#include <span>
#include <numbers>

namespace ot::egfx
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

		enum class split_fail
		{
			inside, // the face was entirely inside of the half-space defined by the input plane
			outside, // the face was entirely outside of the half-space defined by the input plane
			aligned, // the face's plane and normal is aligned with the input plane
			opposite_aligned, // the face's plane is aligned with the input plane, but the normals are opposite
		};
	}

	namespace detail
	{
		template<typename RefType>
		class cref_range;

		template<typename RefType>
		class ref_range;

		template<template<typename Iterator> typename Iteration>
		class half_edge_range;

		template<template<typename Iterator> typename Iteration>
		class const_half_edge_range;

		template<typename Iterator>
		class face_half_edge_iteration;

		template<typename Iterator>
		class vertex_half_edge_iteration;

		template<typename Iterator>
		class edge_iteration;

		class face_vertex_range;

		class const_face_vertex_range;

		struct vertex_data;
		struct half_edge_data;
		struct face_data;
	}

	namespace vertex
	{
		class cref
		{
			mesh_definition const* m;
			id vertex_id;

		public:
			using id_type = id;
			using mesh_type = mesh_definition const;

			cref() = default;
			cref(mesh_definition const& m, id v) noexcept
				: m(&m)
				, vertex_id(v)
			{

			}

			[[nodiscard]] id get_id() const noexcept { return vertex_id; }

			// Get the position in the mesh's local space of the vertex
			[[nodiscard]] math::point3f get_position() const;

			// Get the UV coordinates of the vertex
			// TODO: need a UV per face, using a Cubic projection
			[[nodiscard]] math::point2f get_uv() const;

			// Returns a range of the half-edges around the given vertex
			[[nodiscard]] auto get_half_edges() const -> detail::const_half_edge_range<detail::vertex_half_edge_iteration>;

			[[nodiscard]] friend bool operator==(cref lhs, cref rhs) noexcept
			{
				return lhs.m == rhs.m && lhs.vertex_id == rhs.vertex_id;
			}

			[[nodiscard]] friend bool operator!=(cref lhs, cref rhs) noexcept
			{
				return !(lhs == rhs);
			}
		};

		class ref 
		{
			mesh_definition* m;
			id v;

		public:
			using id_type = id;
			using mesh_type = mesh_definition;

			ref() = default;
			ref(mesh_definition& m, id v) noexcept
				: m(&m)
				, v(v)
			{

			}

			[[nodiscard]] cref as_const() const noexcept { return { *m, v }; }
			operator cref() const noexcept { return as_const(); }

			[[nodiscard]] id get_id() const noexcept { return v; }

			// Get the position in the mesh's local space of the vertex
			[[nodiscard]] math::point3f get_position() const;

			// Returns a range of the half-edges around the given vertex
			[[nodiscard]] auto get_half_edges() const -> detail::half_edge_range<detail::vertex_half_edge_iteration>;

			[[nodiscard]] friend bool operator==(ref lhs, ref rhs) noexcept
			{
				return lhs.m == rhs.m && lhs.v == rhs.v;
			}

			[[nodiscard]] friend bool operator!=(ref lhs, ref rhs) noexcept
			{
				return !(lhs == rhs);
			}
		};
	}

	namespace half_edge
	{
		class cref
		{
			mesh_definition const* m;
			id e;

		public:
			using id_type = id;
			using mesh_type = mesh_definition const;

			cref() = default;
			cref(mesh_definition const& m, id e)
				: m(&m)
				, e(e)
			{

			}

			[[nodiscard]] id get_id() const noexcept { return e; }

			// Returns the vertex the half-edge originates from
			[[nodiscard]] vertex::cref get_source_vertex() const;

			// Returns the vertex the half-edge is pointing to
			[[nodiscard]] vertex::cref get_target_vertex() const;

			// Returns the face the half-edge is on
			[[nodiscard]] face::cref get_face() const;

			// Returns the half-edge's twin
			[[nodiscard]] cref get_twin() const;

			// Returns the line from the source vertex to the target vertex
			[[nodiscard]] math::line get_line() const;

			// Returns whether the half-edge is the "primary" unit in the pair.
			// Mostly useful for traversing each edge once
			[[nodiscard]] bool is_primary() const;

			// Returns the next half-edge along the face
			// Useful for iterating
			[[nodiscard]] half_edge::cref get_next() const;

			[[nodiscard]] friend bool operator==(cref lhs, cref rhs) noexcept
			{
				return lhs.m == rhs.m && lhs.e == rhs.e;
			}

			[[nodiscard]] friend bool operator!=(cref lhs, cref rhs) noexcept
			{
				return !(lhs == rhs);
			}
		};

		class ref 
		{
			mesh_definition* m;
			id e;

		public:
			using id_type = id;
			using mesh_type = mesh_definition;

			ref() = default;
			ref(mesh_definition& m, id e)
				: m(&m)
				, e(e)
			{

			}

			[[nodiscard]] cref as_const() const noexcept { return { *m, e }; }
			operator cref() const noexcept { return as_const(); }

			[[nodiscard]] id get_id() const noexcept { return e; }

			// Returns the vertex the half-edge originates from
			[[nodiscard]] vertex::ref get_source_vertex() const;

			// Returns the vertex the half-edge is pointing to
			[[nodiscard]] vertex::ref get_target_vertex() const;

			// Returns the face the half-edge is on
			[[nodiscard]] face::ref get_face() const;

			// Returns the half-edge's twin
			[[nodiscard]] ref get_twin() const;

			// Returns the line from the source vertex to the target vertex
			[[nodiscard]] math::line get_line() const;

			// Returns whether the half-edge is the "primary" unit in the pair.
			// Mostly useful for traversing each edge once
			[[nodiscard]] bool is_primary() const;

			// Returns the next half-edge along the face
			// Useful for iterating
			[[nodiscard]] half_edge::ref get_next() const;

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
			ref split_at(math::point3f point) const;

			[[nodiscard]] friend bool operator==(ref lhs, ref rhs) noexcept
			{
				return lhs.m == rhs.m && lhs.e == rhs.e;
			}

			[[nodiscard]] friend bool operator!=(ref lhs, ref rhs) noexcept
			{
				return !(lhs == rhs);
			}
		};
	}

	namespace face
	{
		class cref
		{
			mesh_definition const* m;
			id f;

			detail::face_data const& get_face_data() const noexcept;

		public:
			using id_type = id;
			using mesh_type = mesh_definition const;

			cref() = default;
			cref(mesh_definition const& m, id f)
				: m(&m)
				, f(f)
			{

			}

			[[nodiscard]] id get_id() const noexcept { return f; }

			// Returns the normal vector of the face
			[[nodiscard]] math::vector3f get_normal() const;

			// Returns a range of the half-edges around the given face
			[[nodiscard]] auto get_half_edges() const -> detail::const_half_edge_range<detail::face_half_edge_iteration>;

			// Returns a range of vertices around the given face
			[[nodiscard]] auto get_vertices() const->detail::const_face_vertex_range;

			// Returns the number of vertices on the face
			[[nodiscard]] size_t get_vertex_count() const;

			// Returns the plane parallel to the face
			[[nodiscard]] math::plane get_plane() const;

			// Returns whether the point is on the face. 
			// The point must be in the local coordinates of the mesh definition
			[[nodiscard]] bool is_on_face(math::point3f p) const;

			// Returns an arbitrary half-edge along the face
			// Useful for iterating
			[[nodiscard]] half_edge::cref get_first_half_edge() const;

			[[nodiscard]] friend bool operator==(cref lhs, cref rhs) noexcept
			{
				return lhs.m == rhs.m && lhs.f == rhs.f;
			}

			[[nodiscard]] friend bool operator!=(cref lhs, cref rhs) noexcept
			{
				return !(lhs == rhs);
			}
		};

		class ref
		{
			mesh_definition* m;
			id f;

			detail::face_data& get_face_data() const noexcept;

		public:
			using id_type = id;
			using mesh_type = mesh_definition;

			ref() = default;
			ref(mesh_definition& m, id f)
				: m(&m)
				, f(f)
			{

			}

			[[nodiscard]] cref as_const() const noexcept { return { *m, f }; }
			operator cref() const noexcept { return as_const(); }

			[[nodiscard]] id get_id() const noexcept { return f; }

			// Returns the normal vector of the face
			[[nodiscard]] math::vector3f get_normal() const { return as_const().get_normal(); }

			// Returns a range of the half-edges around the given face
			[[nodiscard]] auto get_half_edges() const -> detail::half_edge_range<detail::face_half_edge_iteration>;

			// Returns a range of vertices around the given face
			[[nodiscard]] auto get_vertices() const -> detail::face_vertex_range;

			// Returns the number of vertices on the face
			[[nodiscard]] size_t get_vertex_count() const;

			// Returns the plane parallel to the face
			[[nodiscard]] math::plane get_plane() const { return as_const().get_plane(); }

			// Returns whether the point is on the face. 
			// The point must be in the local coordinates of the mesh definition
			[[nodiscard]] bool is_on_face(math::point3f p) const { return as_const().is_on_face(p); }

			// Returns an arbitrary half-edge along the face
			// Useful for iterating
			[[nodiscard]] half_edge::ref get_first_half_edge() const;

			// Splits the face in two, between the two sides of the input plane, and returns the new face.
			// If the face is entirely inside or outside the half-space of the plane, or aligned (or opposite-aligned) with the input plane,
			// then no split happens
			expected<ref, split_fail> split(math::plane p) const;

			[[nodiscard]] friend bool operator==(ref lhs, ref rhs) noexcept
			{
				return lhs.m == rhs.m && lhs.f == rhs.f;
			}

			[[nodiscard]] friend bool operator!=(ref lhs, ref rhs) noexcept
			{
				return !(lhs == rhs);
			}
		};
	}

	// A mesh representing a three-dimensional manifold formed of faces, vertices, and pairs of half-edges between each vertex
	// 
	class mesh_definition
	{
		using vertex_data = detail::vertex_data;
		using half_edge_data = detail::half_edge_data;
		using face_data = detail::face_data;

		std::vector<vertex_data> vertices;
		std::vector<half_edge_data> half_edges;
		std::vector<face_data> faces;
		math::aabb bounds{};

		// Accessors
		[[nodiscard]] half_edge_data& get_half_edge_data(half_edge::id id) { return half_edges[static_cast<size_t>(id)]; }
		[[nodiscard]] half_edge_data const& get_half_edge_data(half_edge::id id) const { return half_edges[static_cast<size_t>(id)]; }
		[[nodiscard]] vertex_data& get_vertex_data(vertex::id id) { return vertices[static_cast<size_t>(id)]; }
		[[nodiscard]] vertex_data const& get_vertex_data(vertex::id id) const { return vertices[static_cast<size_t>(id)]; }
		[[nodiscard]] face_data& get_face_data(face::id id) { return faces[static_cast<size_t>(id)]; }
		[[nodiscard]] face_data const& get_face_data(face::id id) const { return faces[static_cast<size_t>(id)]; }

	public:
		mesh_definition() = default;
		// Constructs a mesh from a sequence of planes
		// The mesh will have as many faces as the number of input planes, and the faces will preserve the same order as the plane with the same normal
		mesh_definition(std::span<const math::plane> planes);

		[[nodiscard]] vertex::cref get_vertex(vertex::id id) const noexcept { return { *this, id }; }
		[[nodiscard]] vertex::ref get_vertex(vertex::id id) noexcept { return { *this, id }; }
		[[nodiscard]] half_edge::cref get_half_edge(half_edge::id id) const noexcept { return { *this, id }; }
		[[nodiscard]] half_edge::ref get_half_edge(half_edge::id id) noexcept { return { *this, id }; }
		[[nodiscard]] face::cref get_face(face::id id) const noexcept { return { *this, id }; }
		[[nodiscard]] face::ref get_face(face::id id) noexcept { return { *this, id }; }
		[[nodiscard]] auto get_vertices() const noexcept -> detail::ref_range<vertex::cref>;
		[[nodiscard]] auto get_vertices() noexcept -> detail::ref_range<vertex::ref>;
		[[nodiscard]] auto get_half_edges() const noexcept -> detail::ref_range<half_edge::cref>;
		[[nodiscard]] auto get_half_edges() noexcept -> detail::ref_range<half_edge::ref>;
		[[nodiscard]] auto get_faces() const noexcept -> detail::ref_range<face::cref>;
		[[nodiscard]] auto get_faces() noexcept -> detail::ref_range<face::ref>;
		[[nodiscard]] math::aabb get_bounds() const noexcept { return bounds; }

		// Special range returning only a single half-edge per edge
		// Useful for traversing each edge only once
		[[nodiscard]] auto get_edges() const noexcept -> detail::const_half_edge_range<detail::edge_iteration>;
		
		// Get a basic mesh definition for a cube
		[[nodiscard]] static mesh_definition const& get_cube();

	private:
		template<template<typename Iterator> typename Iteration>
		friend class detail::const_half_edge_range;
		
		template<template<typename Iterator> typename Iteration>
		friend class detail::half_edge_range;

		template<typename Iterator>
		friend class detail::face_half_edge_iteration;

		template<typename Iterator>
		friend class detail::vertex_half_edge_iteration;

		friend class detail::const_face_vertex_range;
		friend class detail::face_vertex_range;

		friend class vertex::cref;
		friend class vertex::ref;
		friend class half_edge::cref;
		friend class half_edge::ref;
		friend class face::cref;
		friend class face::ref;

		struct point_intersection;
		static std::vector<point_intersection> find_intersections(std::span<const math::plane> planes, std::vector<vertex_data>& vertices, std::vector<half_edge_data>& half_edges);
		static void resolve_edge_directions(std::span<const math::plane> planes, std::span<point_intersection const> intersections, std::span<half_edge_data> half_edges, std::span<face_data> faces);
		static void update_bounds(math::aabb& bounds, std::span<vertex_data const> vertices);
	};

	namespace detail
	{
		template<typename RefType>
		class ref_range
		{
			using id_type = typename RefType::id_type;
			using mesh_type = typename RefType::mesh_type;

			mesh_type* mesh;
			id_type first;
			id_type after_last;
		public:
			using value_type = RefType;

			ref_range(mesh_type& mesh, id_type first, id_type after_last)
				: mesh(&mesh)
				, first(first)
				, after_last(after_last)
			{

			}

			class iterator
			{
				mesh_type* mesh;
				id_type id;
			public:
				explicit iterator(mesh_type& mesh, id_type id)
					: mesh(&mesh)
					, id(id)
				{

				}

				using value_type = ref_range::value_type;
				using reference = value_type;
				using pointer = value_type const*;
				using difference_type = ptrdiff_t;
				using iterator_category = std::input_iterator_tag;

				reference operator*() const
				{
					return { *mesh, id };
				}
				arrow_proxy<value_type> operator->() const
				{
					return { { *mesh, id } };
				}
				iterator& operator++()
				{
					id = id_type(static_cast<size_t>(id) + 1);
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

			[[nodiscard]] iterator begin() const noexcept { return iterator{ *mesh, first }; }
			[[nodiscard]] iterator end() const noexcept { return iterator{ *mesh, after_last }; }
			[[nodiscard]] size_t size() const noexcept { return static_cast<size_t>(after_last) - static_cast<size_t>(first); }
			[[nodiscard]] value_type operator[](size_t i) const noexcept { return { *mesh, id_type(i) }; }
			[[nodiscard]] value_type operator[](id_type i) const noexcept { return { *mesh, i }; }
		};

		template<template<typename Iterator> typename Iteration >
		class const_half_edge_range
		{
			mesh_definition const* m = nullptr;
			half_edge::id first = half_edge::id::none;

		public:
			using value_type = half_edge::cref;

			const_half_edge_range() = default;
			const_half_edge_range(mesh_definition const& m, half_edge::id first) noexcept
				: m(&m)
				, first(first)
			{

			}

			class iterator : public Iteration<iterator>
			{
				mesh_definition const* m = nullptr;
				half_edge::id first = half_edge::id::none;
				half_edge::id current = half_edge::id::none;

				friend Iteration<iterator>;

			public:
				iterator() = default;
				iterator(mesh_definition const& m, half_edge::id first, half_edge::id current) noexcept
					: m(&m)
					, first(first)
					, current(current)
				{

				}

				using value_type = const_half_edge_range::value_type;
				using reference = value_type;
				using pointer = value_type*;
				using difference_type = size_t;
				using iterator_category = std::input_iterator_tag;

				reference operator*() const
				{
					return { *m, current };
				}
				arrow_proxy<value_type> operator->() const
				{
					return { { *m, current } };
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

			[[nodiscard]] iterator begin() const { return { *m, first, first }; }
			[[nodiscard]] iterator end() const { return { *m, first, half_edge::id::none }; }
		};

		template<template<typename Iterator> typename Iteration >
		class half_edge_range
		{
			mesh_definition* m = nullptr;
			half_edge::id first = half_edge::id::none;

		public:
			using value_type = half_edge::ref;

			half_edge_range() = default;
			half_edge_range(mesh_definition& m, half_edge::id first) noexcept
				: m(&m)
				, first(first)
			{

			}

			class iterator : public Iteration<iterator>
			{
				mesh_definition* m = nullptr;
				half_edge::id first = half_edge::id::none;
				half_edge::id current = half_edge::id::none;

				friend Iteration<iterator>;

			public:
				iterator() = default;
				iterator(mesh_definition& m, half_edge::id first, half_edge::id current) noexcept
					: m(&m)
					, first(first)
					, current(current)
				{

				}

				using value_type = half_edge_range::value_type;
				using reference = value_type;
				using pointer = value_type*;
				using difference_type = size_t;
				using iterator_category = std::input_iterator_tag;

				reference operator*() const
				{
					return { *m, current };
				}
				arrow_proxy<value_type> operator->() const
				{
					return { { *m, current } };
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

			[[nodiscard]] iterator begin() const { return { *m, first, first }; }
			[[nodiscard]] iterator end() const { return { *m, first, half_edge::id::none }; }
		};

		template<typename Iterator>
		class face_half_edge_iteration
		{
		public:
			using derived = Iterator;
			auto operator++() -> derived&
			{
				auto& self = static_cast<derived&>(*this);

				auto const next = self.m->get_half_edge_data(self.current).next;
				if (next == self.first)
				{
					self.current = half_edge::id::none;
				} 
				else
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

				auto const next = self.m->get_half_edge_data(self.m->get_half_edge_data(self.current).twin).next;
				if (next == self.first)
				{
					self.current = half_edge::id::none;
				} 
				else
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
		class edge_iteration
		{
		public:
			using derived = Iterator;
			auto operator++() -> derived&
			{
				auto& self = static_cast<derived&>(*this);

				auto const half_edges = self.m->get_half_edges();

				half_edge::id next = self.current;
				do
				{
					next = static_cast<half_edge::id>(static_cast<size_t>(next) + 1);
				} while (static_cast<size_t>(next) != half_edges.size() && !self.m->get_half_edge(next).is_primary());
				
				if(static_cast<size_t>(next) == half_edges.size())
				{
					self.current = half_edge::id::none;
				}
				else
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
			mesh_definition* m = nullptr;
			half_edge::id first = half_edge::id::none;

		public:
			using value_type = vertex::id;

			face_vertex_range() = default;
			face_vertex_range(mesh_definition& m, half_edge::id first)
				: m(&m)
				, first(first)
			{

			}

			class iterator : public detail::face_half_edge_iteration<iterator>
			{
				mesh_definition* m = nullptr;
				half_edge::id first = half_edge::id::none;
				half_edge::id current = half_edge::id::none;

				friend face_half_edge_iteration<iterator>;

			public:
				iterator() = default;
				iterator(mesh_definition& m, half_edge::id first, half_edge::id current) noexcept
					: m(&m)
					, first(first)
					, current(current)
				{

				}

				using value_type = vertex::ref;
				using reference = value_type;
				using pointer = value_type*;
				using difference_type = size_t;
				using iterator_category = std::forward_iterator_tag;

				reference operator*() const
				{
					return half_edge::ref{ *m, current }.get_source_vertex();
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

			[[nodiscard]] iterator begin() const noexcept { return { *m, first, first }; }
			[[nodiscard]] iterator end() const noexcept { return { *m, first, half_edge::id::none }; }
		};

		class const_face_vertex_range
		{
			mesh_definition const* m = nullptr;
			half_edge::id first = half_edge::id::none;

		public:
			using value_type = vertex::id;

			const_face_vertex_range() = default;
			const_face_vertex_range(mesh_definition const& m, half_edge::id first)
				: m(&m)
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
				iterator(mesh_definition const& m, half_edge::id first, half_edge::id current) noexcept
					: m(&m)
					, first(first)
					, current(current)
				{

				}

				using value_type = vertex::cref;
				using reference = value_type;
				using pointer = value_type*;
				using difference_type = size_t;
				using iterator_category = std::forward_iterator_tag;

				reference operator*() const
				{
					return half_edge::cref{ *m, current }.get_source_vertex();
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

			[[nodiscard]] iterator begin() const noexcept { return { *m, first, first }; }
			[[nodiscard]] iterator end() const noexcept { return { *m, first, half_edge::id::none }; }
		};

		struct vertex_data
		{
			math::point3f position;
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
			math::vector3f normal;
		};
	}

	inline auto mesh_definition::get_vertices() const noexcept -> detail::ref_range<vertex::cref>
	{
		return { *this, vertex::id(0), vertex::id(vertices.size()) };
	}

	inline auto mesh_definition::get_vertices() noexcept -> detail::ref_range<vertex::ref>
	{
		return { *this, vertex::id(0), vertex::id(vertices.size()) };
	}

	inline auto mesh_definition::get_faces() const noexcept -> detail::ref_range<face::cref>
	{
		return { *this, face::id(0), face::id(faces.size()) };
	}

	inline auto mesh_definition::get_faces() noexcept -> detail::ref_range<face::ref>
	{
		return { *this, face::id(0), face::id(faces.size()) };
	}

	inline auto mesh_definition::get_half_edges() const noexcept -> detail::ref_range<half_edge::cref>
	{
		return { *this, half_edge::id(0), half_edge::id(half_edges.size()) };
	}

	inline auto mesh_definition::get_half_edges() noexcept -> detail::ref_range<half_edge::ref>
	{
		return { *this, half_edge::id(0), half_edge::id(half_edges.size()) };
	}

	inline auto mesh_definition::get_edges() const noexcept -> detail::const_half_edge_range<detail::edge_iteration>
	{
		return { *this, half_edge::id(0) };
	}

	namespace vertex
	{
		inline math::point3f cref::get_position() const
		{
			return m->get_vertex_data(vertex_id).position;
		}

		inline auto cref::get_half_edges() const -> detail::const_half_edge_range<detail::vertex_half_edge_iteration>
		{
			return { *m, m->get_vertex_data(vertex_id).first_edge };
		}

		inline math::point3f ref::get_position() const
		{
			return as_const().get_position();
		}

		inline auto ref::get_half_edges() const -> detail::half_edge_range<detail::vertex_half_edge_iteration>
		{
			return { *m, m->get_vertex_data(v).first_edge };
		}
	}

	namespace half_edge
	{
		inline vertex::cref cref::get_source_vertex() const
		{
			return { *m, m->get_half_edge_data(m->get_half_edge_data(e).twin).vertex };
		}

		inline vertex::cref cref::get_target_vertex() const
		{
			return { *m, m->get_half_edge_data(e).vertex };
		}

		inline cref cref::get_twin() const
		{
			return { *m, m->get_half_edge_data(e).twin };
		}

		inline face::cref cref::get_face() const
		{
			return { *m, m->get_half_edge_data(e).face };
		}

		inline math::line cref::get_line() const
		{
			return { get_source_vertex().get_position(), get_target_vertex().get_position() };
		}

		inline bool cref::is_primary() const
		{
			// As an arbitrary discriminator, the primary half-edge is the one with the smallest face id
			half_edge::cref const twin = get_twin();
			return m->get_half_edge_data(get_id()).face < m->get_half_edge_data(twin.get_id()).face;
		}

		inline half_edge::cref cref::get_next() const
		{
			return m->get_half_edge(m->get_half_edge_data(e).next);
		}

		inline vertex::ref ref::get_source_vertex() const
		{
			return { *m, m->get_half_edge_data(m->get_half_edge_data(e).twin).vertex };
		}

		inline vertex::ref ref::get_target_vertex() const
		{
			return { *m, m->get_half_edge_data(e).vertex };
		}

		inline ref ref::get_twin() const
		{
			return { *m, m->get_half_edge_data(e).twin };
		}

		inline face::ref ref::get_face() const
		{
			return m->get_face(m->get_half_edge_data(e).face);
		}

		inline math::line ref::get_line() const
		{
			return as_const().get_line();
		}

		inline bool ref::is_primary() const
		{
			return as_const().is_primary();
		}

		inline half_edge::ref ref::get_next() const
		{
			return m->get_half_edge(m->get_half_edge_data(e).next);
		}
	}

	namespace face
	{
		inline auto cref::get_half_edges() const -> detail::const_half_edge_range<detail::face_half_edge_iteration>
		{
			return { *m, m->get_face_data(f).first_edge };
		}

		inline auto cref::get_vertices() const -> detail::const_face_vertex_range
		{
			return { *m, m->get_face_data(f).first_edge };
		}

		inline size_t cref::get_vertex_count() const
		{
			auto const he = get_half_edges();
			return std::distance(he.begin(), he.end());
		}

		inline auto ref::get_half_edges() const -> detail::half_edge_range<detail::face_half_edge_iteration>
		{
			return { *m, m->get_face_data(f).first_edge };
		}

		inline auto ref::get_vertices() const -> detail::face_vertex_range
		{
			return { *m, m->get_face_data(f).first_edge };
		}

		inline size_t ref::get_vertex_count() const
		{
			return as_const().get_vertex_count();
		}
	}
}
