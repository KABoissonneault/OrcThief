#include "static_mesh.h"

#include "Ogre/Root.h"
#include "Ogre/MemoryAllocatorConfig.h"
#include "Ogre/MeshManager2.h"
#include "Ogre/SubMesh2.h"
#include "Ogre/Vao/Manager.h"

namespace ot::graphics
{
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

		[[nodiscard]] Ogre::Vector3 to_ogre_vector(math::point3d p)
		{
			return { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) };
		}

		[[nodiscard]] Ogre::Vector3 to_ogre_vector(math::vector3d p)
		{
			return { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) };
		}

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

		[[nodiscard]] Ogre::IndexBufferPacked* make_render_index_buffer(geometry_data data)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto const index_buffer = vao_manager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT, data.size, Ogre::BT_IMMUTABLE, data.buffer.get(), true /* take ownership */);
			data.buffer.release();
			return index_buffer;
		}

		[[nodiscard]] std::pair<size_t, size_t> get_triangle_vertex_index_count(math::mesh const& mesh)
		{
			size_t vertex_count = 0;
			size_t index_count = 0;
			for (auto const& face : mesh.get_faces())
			{
				auto const face_vertex_count = get_vertex_count(mesh, face);
				auto const triangle_count = face_vertex_count - 2;

				vertex_count += face_vertex_count;
				index_count += triangle_count * 3;
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
		[[nodiscard]] auto make_triangle_data(math::mesh const& mesh) -> vertex_array_data
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

		[[nodiscard]] Ogre::VertexArrayObject* make_render_vao(math::mesh const& mesh)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto [vertex_data, index_data] = make_triangle_data(mesh);
			
			auto const vertex_buffers = make_render_vertex_buffers(std::move(vertex_data));
			auto const index_buffer = make_render_index_buffer(std::move(index_data));

			return vao_manager->createVertexArrayObject(vertex_buffers, index_buffer, Ogre::OT_TRIANGLE_LIST);
		}
	}

	Ogre::MeshPtr make_static_mesh(ogre::string const& name, math::mesh const& mesh)
	{
		Ogre::MeshPtr const render_mesh = Ogre::MeshManager::getSingleton().createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::SubMesh* render_submesh = render_mesh->createSubMesh();
		auto const vao = make_render_vao(mesh);
		render_submesh->mVao[Ogre::VpNormal].push_back(vao);
		render_submesh->mVao[Ogre::VpShadow].push_back(vao);

		math::aabb const mesh_bounds = mesh.get_bounds();
		math::point3d const bounds_center = mesh_bounds.position;
		math::vector3d const bounds_half_size = mesh_bounds.half_size;
		render_mesh->_setBounds(Ogre::Aabb(to_ogre_vector(bounds_center), to_ogre_vector(bounds_half_size)), false);
		render_mesh->_setBoundingSphereRadius(bounds_half_size.norm());

		return render_mesh;
	}
}
