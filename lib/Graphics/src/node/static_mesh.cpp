#include "static_mesh.h"

#include "scene.h"
#include "ogre_conversion.h"
#include "node/object.h"
#include "graphics/mesh_definition.h"

#include "Ogre/Root.h"
#include "Ogre/MemoryAllocatorConfig.h"
#include "Ogre/Vao/Manager.h"
#include "Ogre/MeshManager2.h"
#include "Ogre/SubMesh2.h"
#include "Ogre/Item.h"

namespace ot::graphics::node
{
	namespace detail
	{
		void init_static_mesh_impl(static_mesh& smesh, void* snode_data, void* mesh_ptr) noexcept
		{
			auto const snode = static_cast<Ogre::SceneNode*>(snode_data);
			auto& mesh = *static_cast<Ogre::MeshPtr*>(mesh_ptr);

			init_object(smesh, snode);
			get_mesh_ptr(smesh) = std::move(mesh);
		}

		void* get_mesh_ptr_impl(static_mesh& smesh) noexcept
		{
			return &smesh.storage_mesh;
		}
	}

	void init_static_mesh(static_mesh& smesh, Ogre::SceneNode* snode, Ogre::MeshPtr mptr) noexcept
	{
		detail::init_static_mesh_impl(smesh, snode, &mptr);
	}	

	Ogre::MeshPtr& get_mesh_ptr(static_mesh& smesh) noexcept
	{
		return *static_cast<Ogre::MeshPtr*>(detail::get_mesh_ptr_impl(smesh));
	}

	Ogre::MeshPtr&& get_mesh_ptr(static_mesh&& smesh) noexcept
	{
		return std::move(*static_cast<Ogre::MeshPtr*>(detail::get_mesh_ptr_impl(smesh)));
	}

	static_mesh::static_mesh() noexcept
	{
		static_assert(offsetof(static_mesh, storage_mesh) == sizeof(object) && alignof(static_mesh) == alignof(Ogre::MeshPtr));
		static_assert(sizeof(static_mesh::storage_mesh) == sizeof(Ogre::MeshPtr));

		new(storage_mesh) Ogre::MeshPtr;
	}

	static_mesh::static_mesh(static_mesh&& other) noexcept
		: object(std::move(other))
	{
		new(storage_mesh) Ogre::MeshPtr(get_mesh_ptr(std::move(other)));
	}

	static_mesh& static_mesh::operator=(static_mesh&& other) noexcept
	{
		if (this != &other)
		{
			destroy_mesh();
			static_cast<object&>(*this) = std::move(other);
			get_mesh_ptr(*this) = get_mesh_ptr(std::move(other));
		}
		return *this;
	}

	static_mesh::~static_mesh()
	{
		destroy_mesh();
		get_mesh_ptr(*this).~SharedPtr();
	}

	void static_mesh::destroy_mesh() noexcept
	{
		auto& mesh_ptr = get_mesh_ptr(*this);
		if (mesh_ptr != nullptr)
		{
			auto& mesh_manager = Ogre::MeshManager::getSingleton();
			mesh_manager.remove(mesh_ptr);
		}
	}

	namespace
	{
		struct render_vertex
		{
			Ogre::Vector3 position;
			Ogre::Vector3 normal;

			[[nodiscard]] static Ogre::VertexElement2Vec get_vertex_buffer_elements()
			{
				// Vertex buffers seem to only support floats, not doubles
				return {
					Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION)
					, Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL)
				};
			}
		};

		struct wireframe_vertex
		{
			Ogre::Vector3 position;

			[[nodiscard]] static Ogre::VertexElement2Vec get_vertex_buffer_elements()
			{
				return {
					Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION)
				};
			}
		};

		// Creates a static vertex buffer 
		[[nodiscard]] Ogre::VertexBufferPacked* create_static_vertex_buffer(Ogre::VertexElement2Vec const& buffer_elements, size_t num_vertices, ogre::unique_geometry_mem initial_data)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto const vertex_buffer = vao_manager->createVertexBuffer(buffer_elements, num_vertices, Ogre::BT_IMMUTABLE, initial_data.get(), true /* take ownership */);
			initial_data.release();
			return vertex_buffer;
		}

		struct geometry_data
		{
			ogre::unique_geometry_mem buffer;
			size_t size; // in element count
		};

		[[nodiscard]] Ogre::VertexBufferPackedVec make_render_vertex_buffers(geometry_data data)
		{
			Ogre::VertexBufferPacked* const vertex_buffer = create_static_vertex_buffer(render_vertex::get_vertex_buffer_elements(), data.size, std::move(data).buffer);
			return { vertex_buffer };
		}

		[[nodiscard]] Ogre::VertexBufferPackedVec make_wireframe_vertex_buffers(geometry_data data)
		{
			Ogre::VertexBufferPacked* const vertex_buffer = create_static_vertex_buffer(wireframe_vertex::get_vertex_buffer_elements(), data.size, std::move(data).buffer);
			return { vertex_buffer };
		}

		[[nodiscard]] Ogre::IndexBufferPacked* make_index_buffer(geometry_data data)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto const index_buffer = vao_manager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT, data.size, Ogre::BT_IMMUTABLE, data.buffer.get(), true /* take ownership */);
			data.buffer.release();
			return index_buffer;
		}

		[[nodiscard]] std::pair<size_t, size_t> get_triangle_vertex_index_count(mesh_definition const& mesh)
		{
			size_t vertex_count = 0;
			size_t index_count = 0;
			for (auto const& face : mesh.get_faces())
			{
				auto const face_vertex_count = get_vertex_count(mesh, face);
				assert(face_vertex_count >= 3); // I've had issues at some point, better make sure
				auto const triangle_count = face_vertex_count - 2;

				vertex_count += face_vertex_count;
				index_count += triangle_count * 3;
			}
			return { vertex_count, index_count };
		}

		[[nodiscard]] std::pair<size_t, size_t> get_wireframe_vertex_index_count(mesh_definition const& mesh)
		{
			size_t vertex_count = 0;
			size_t index_count = 0;
			for (auto const& face : mesh.get_faces())
			{
				auto const face_vertex_count = get_vertex_count(mesh, face);
				auto const edge_count = face_vertex_count;

				vertex_count += face_vertex_count;
				index_count += edge_count * 2;
			}
			return { vertex_count, index_count };
		}

		struct vertex_array_data
		{
			geometry_data vertex_data;
			geometry_data index_data;
		};

		// Builds a fan-shaped triangle list
		// Triangle fans are not support on Direct3D 11, and we need something that works for the whole mesh anyway
		[[nodiscard]] auto make_triangle_data(mesh_definition const& mesh) -> vertex_array_data
		{
			auto const [vertex_count, index_count] = get_triangle_vertex_index_count(mesh);

			ogre::unique_geometry_mem vertex_mem = ogre::allocate_geometry(sizeof(render_vertex) * vertex_count);
			ogre::unique_geometry_mem index_mem = ogre::allocate_geometry(index_count * sizeof(Ogre::uint16));
			auto const vertex_mem_begin = reinterpret_cast<render_vertex*>(vertex_mem.get());
			auto vertex_it = vertex_mem_begin;
			auto index_it = reinterpret_cast<Ogre::uint16*>(index_mem.get());
			
			for (auto const& face : mesh.get_faces())
			{
				auto const vertices = get_vertices(mesh, face);
				auto const face_vertex_count = get_vertex_count(mesh, face);
				auto const normal = to_ogre_vector(get_normal(mesh, face));

				auto const base_index = static_cast<Ogre::uint16>(std::distance(vertex_mem_begin, vertex_it));

				// push vertices
				for (auto const vertex : vertices)
				{
					new(vertex_it++) render_vertex{ to_ogre_vector(get_position(mesh, vertex)), normal };
				}

				// push indices
				auto const index1 = base_index;
				auto index2 = base_index + 1;
				
				do
				{
					auto index3 = index2 + 1;
					new(index_it++) auto(static_cast<Ogre::uint16>(index1));
					new(index_it++) auto(static_cast<Ogre::uint16>(index2));
					new(index_it++) auto(static_cast<Ogre::uint16>(index3));
					index2 = index3;
				} while (index2 - index1 + 1 != face_vertex_count);
			}

			return
			{
				{ std::move(vertex_mem), vertex_count }
				, { std::move(index_mem), index_count }
			};
		}

		[[nodiscard]] auto make_wireframe_data(mesh_definition const& mesh) -> vertex_array_data
		{
			auto const [vertex_count, index_count] = get_wireframe_vertex_index_count(mesh);

			ogre::unique_geometry_mem vertex_mem = ogre::allocate_geometry(sizeof(wireframe_vertex) * vertex_count);
			ogre::unique_geometry_mem index_mem = ogre::allocate_geometry(index_count * sizeof(Ogre::uint16));
			auto const vertex_mem_begin = reinterpret_cast<wireframe_vertex*>(vertex_mem.get());
			auto vertex_it = vertex_mem_begin;
			auto index_it = reinterpret_cast<Ogre::uint16*>(index_mem.get());

			for (auto const& face : mesh.get_faces())
			{
				auto const vertices = get_vertices(mesh, face);
				auto const face_vertex_count = static_cast<Ogre::uint16>(get_vertex_count(mesh, face));

				auto const base_index = static_cast<Ogre::uint16>(std::distance(vertex_mem_begin, vertex_it));

				// push vertices
				for (auto const vertex : vertices)
				{
					new(vertex_it++) wireframe_vertex{ to_ogre_vector(get_position(mesh, vertex)) };
				}

				// push indices
				for (Ogre::uint16 i = 0; i < face_vertex_count - 1; ++i) {
					new(index_it++) Ogre::uint16(base_index + i);
					new(index_it++) Ogre::uint16(base_index + i + 1);
				}

				new(index_it++) Ogre::uint16(base_index + face_vertex_count - 1);
				new(index_it++) Ogre::uint16(base_index);
			}

			return
			{
				{ std::move(vertex_mem), vertex_count }
				, { std::move(index_mem), index_count }
			};
		}

		[[nodiscard]] Ogre::VertexArrayObject* make_render_vao(mesh_definition const& mesh)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto [vertex_data, index_data] = make_triangle_data(mesh);
			
			auto const vertex_buffers = make_render_vertex_buffers(std::move(vertex_data));
			auto const index_buffer = make_index_buffer(std::move(index_data));

			return vao_manager->createVertexArrayObject(vertex_buffers, index_buffer, Ogre::OT_TRIANGLE_LIST);
		}

		[[nodiscard]] Ogre::VertexArrayObject* make_wireframe_vao(mesh_definition const& mesh)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto [vertex_data, index_data] = make_wireframe_data(mesh);

			auto const vertex_buffers = make_wireframe_vertex_buffers(std::move(vertex_data));
			auto const index_buffer = make_index_buffer(std::move(index_data));

			return vao_manager->createVertexArrayObject(vertex_buffers, index_buffer, Ogre::OT_LINE_LIST);
		}

		[[nodiscard]] void set_mesh_bounds(Ogre::Mesh& render_mesh, math::aabb const& mesh_bounds)
		{
			math::point3d const bounds_center = mesh_bounds.position;
			math::vector3d const bounds_half_size = mesh_bounds.half_size;
			render_mesh._setBounds(Ogre::Aabb(to_ogre_vector(bounds_center), to_ogre_vector(bounds_half_size)), false);
			render_mesh._setBoundingSphereRadius(bounds_half_size.norm());
		}
	}

	static_mesh create_static_mesh(object& parent, std::string const& name, mesh_definition const& mesh)
	{
		Ogre::SceneManager& scene_manager = *get_scene_node(parent).getCreator();
		auto& mesh_manager = Ogre::MeshManager::getSingleton();

		Ogre::MeshPtr render_mesh = mesh_manager.createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::SubMesh* render_submesh = render_mesh->createSubMesh();
		auto const vao = make_render_vao(mesh);
		render_submesh->mVao[Ogre::VpNormal].push_back(vao);
		render_submesh->mVao[Ogre::VpShadow].push_back(vao);

		set_mesh_bounds(*render_mesh, mesh.get_bounds());

		Ogre::SceneNode* const root_node = scene_manager.getRootSceneNode(Ogre::SCENE_DYNAMIC);
		Ogre::Item* const item = scene_manager.createItem(render_mesh, Ogre::SCENE_DYNAMIC);
		Ogre::SceneNode* const mesh_node = root_node->createChildSceneNode(Ogre::SCENE_DYNAMIC);
		mesh_node->attachObject(item);

		static_mesh m;
		init_static_mesh(m, mesh_node, std::move(render_mesh));
		return m;
	}

	static_mesh create_static_wireframe_mesh(object& parent, std::string const& name, mesh_definition const& mesh)
	{
		Ogre::SceneManager& scene_manager = *get_scene_node(parent).getCreator();
		auto& mesh_manager = Ogre::MeshManager::getSingleton();

		Ogre::MeshPtr wireframe_mesh = mesh_manager.createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		
		Ogre::SubMesh* wireframe_submesh = wireframe_mesh->createSubMesh();
		auto const vao = make_wireframe_vao(mesh);
		wireframe_submesh->mVao[Ogre::VpNormal].push_back(vao);

		set_mesh_bounds(*wireframe_mesh, mesh.get_bounds());

		Ogre::Item* const item = scene_manager.createItem(wireframe_mesh, Ogre::SCENE_DYNAMIC);
		Ogre::SceneNode* const mesh_node = get_scene_node(parent).createChildSceneNode(Ogre::SCENE_DYNAMIC);
		mesh_node->attachObject(item);

		static_mesh m;
		init_static_mesh(m, mesh_node, std::move(wireframe_mesh));
		return m;
	}
}
