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
		};

		Ogre::Vector3 to_ogre_vector(math::point3d p)
		{
			return { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) };
		}

		Ogre::Vector3 to_ogre_vector(math::vector3d p)
		{
			return { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) };
		}

		[[nodiscard]] Ogre::VertexElement2Vec get_vertex_buffer_elements()
		{
			// Vertex buffers seem to only support floats, not doubles
			return {
				Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION)
				, Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL)
			};
		}

		[[nodiscard]] ogre::unique_geometry_mem make_vertex_data(math::mesh const& mesh)
		{
			auto const vertex_count = mesh.get_vertices().size();
			auto geometry_buffer = ogre::allocate_geometry(sizeof(render_vertex) * vertex_count);
			auto vertex_it = reinterpret_cast<render_vertex*>(geometry_buffer.get());
			for (size_t i = 0; i < vertex_count; ++i)
			{
				auto const& vertex = mesh.get_vertex(math::mesh::vertex_id(i));
				new(vertex_it + i) render_vertex{ to_ogre_vector(vertex.position), to_ogre_vector(vertex.normal) };
			}
			return geometry_buffer;
		}

		// Creates a static vertex buffer 
		[[nodiscard]] Ogre::VertexBufferPacked* create_static_vertex_buffer(size_t num_vertices, ogre::unique_geometry_mem initial_data)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto const vertex_buffer = vao_manager->createVertexBuffer(get_vertex_buffer_elements(), num_vertices, Ogre::BT_IMMUTABLE, initial_data.get(), true /* take ownership */);
			initial_data.release();
			return vertex_buffer;
		}

		[[nodiscard]] Ogre::VertexBufferPackedVec make_vertex_buffers(math::mesh const& mesh)
		{
			ogre::unique_geometry_mem vertex_data = make_vertex_data(mesh);
			Ogre::VertexBufferPacked* const vertex_buffer = create_static_vertex_buffer(mesh.get_vertices().size(), std::move(vertex_data));
			return { vertex_buffer };
		}

		[[nodiscard]] size_t get_index_count(math::mesh const& mesh)
		{
			size_t index_count = 0;
			for (auto const& face : mesh.get_faces())
			{
				auto const triangle_count = mesh.get_face_vertex_count(face) - 2;
				index_count += triangle_count * 3;
			}
			return index_count;
		}

		// Builds a fan-shaped triangle list
		// Triangle fans are not support on Direct3D 11, and we need something that works for the whole mesh anyway
		[[nodiscard]] ogre::unique_geometry_mem make_index_mem(size_t index_count, math::mesh const& mesh)
		{
			ogre::unique_geometry_mem index_mem = ogre::allocate_geometry(index_count * sizeof(Ogre::uint16));
			auto index_it = reinterpret_cast<Ogre::uint16*>(index_mem.get());
			
			for (auto const& face : mesh.get_faces())
			{
				auto const vertices = mesh.get_face_vertex_ids(face);
				auto vertex_it = vertices.begin();
				auto const vertex_sent = vertices.end();

				auto const push_triangle = [&index_it](auto const first_vertex, auto const second_vertex, auto const third_vertex)
				{
					new(index_it++) auto(static_cast<Ogre::uint16>(first_vertex));
					new(index_it++) auto(static_cast<Ogre::uint16>(second_vertex));
					new(index_it++) auto(static_cast<Ogre::uint16>(third_vertex));
				};

				auto const first_vertex = *vertex_it++;
				auto second_vertex = *vertex_it++;				
				
				for (; vertex_it != vertex_sent; ++vertex_it)
				{
					auto third_vertex = *vertex_it;
					push_triangle(first_vertex, second_vertex, third_vertex);
					second_vertex = third_vertex;
				}
			}

			return index_mem;
		}

		// Creates indices for fan triangulation
		[[nodiscard]] Ogre::IndexBufferPacked* make_index_buffer(math::mesh const& mesh)
		{
			size_t const index_count = get_index_count(mesh);
			auto index_mem = make_index_mem(index_count, mesh);

			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto const index_buffer = vao_manager->createIndexBuffer(Ogre::IndexBufferPacked::IT_16BIT, index_count, Ogre::BT_IMMUTABLE, index_mem.get(), true /* take ownership */);
			index_mem.release();
			return index_buffer;
		}

		[[nodiscard]] Ogre::VertexArrayObject* make_vao(math::mesh const& mesh)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const render_system = root.getRenderSystem();
			auto const vao_manager = render_system->getVaoManager();

			auto const vertex_buffers = make_vertex_buffers(mesh);
			auto const index_buffer = make_index_buffer(mesh);
			return vao_manager->createVertexArrayObject(vertex_buffers, index_buffer, Ogre::OT_TRIANGLE_LIST);
		}
	}

	Ogre::MeshPtr make_static_mesh(ogre::string const& name, math::mesh const& mesh)
	{
		Ogre::MeshPtr const render_mesh = Ogre::MeshManager::getSingleton().createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::SubMesh* render_submesh = render_mesh->createSubMesh();
		auto const vao = make_vao(mesh);
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
