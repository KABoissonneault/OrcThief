#include "mesh.h"

#include "scene.h"
#include "ogre_conversion.h"
#include "node.h"
#include "egfx/mesh_definition.h"
#include "material.h"

#include "Ogre/Root.h"
#include "Ogre/MemoryAllocatorConfig.h"
#include "Ogre/Vao/Manager.h"
#include "Ogre/MeshManager2.h"
#include "Ogre/SubMesh2.h"
#include "Ogre/Item.h"

namespace ot::egfx
{
	namespace detail
	{
		void init_mesh_impl(mesh& smesh, void* mesh_ptr) noexcept
		{
			auto& mesh = *static_cast<Ogre::MeshPtr*>(mesh_ptr);

			get_mesh_ptr(smesh) = std::move(mesh);
		}

		void const* get_mesh_ptr_impl(mesh const& smesh) noexcept
		{
			return &smesh.storage_mesh;
		}

		item_ref make_item_ref(void* pimpl) noexcept
		{
			item_ref ref;
			init_object_ref(ref, pimpl);
			return ref;
		}

		item_cref make_item_cref(void const* pimpl) noexcept
		{
			item_cref ref;
			init_object_cref(ref, pimpl);
			return ref;
		}
	}

	void init_mesh(mesh& smesh, Ogre::MeshPtr mptr) noexcept
	{
		detail::init_mesh_impl(smesh, &mptr);
	}

	Ogre::MeshPtr const& get_mesh_ptr(mesh const& smesh) noexcept
	{
		return *static_cast<Ogre::MeshPtr const*>(detail::get_mesh_ptr_impl(smesh));
	}

	Ogre::MeshPtr& get_mesh_ptr(mesh& smesh) noexcept
	{
		return const_cast<Ogre::MeshPtr&>(get_mesh_ptr(static_cast<mesh const&>(smesh)));
	}

	Ogre::MeshPtr&& get_mesh_ptr(mesh&& smesh) noexcept
	{
		return const_cast<Ogre::MeshPtr&&>(get_mesh_ptr(static_cast<mesh const&>(smesh)));
	}

	item_ref make_item_ref(Ogre::Item& item) noexcept
	{
		return detail::make_item_ref(&item);
	}

	item_cref make_item_cref(Ogre::Item const& item) noexcept
	{
		return detail::make_item_cref(&item);
	}

	Ogre::Item& get_item(item_ref i) noexcept
	{
		return static_cast<Ogre::Item&>(get_object(i));
	}

	Ogre::Item const& get_item(item_cref i) noexcept
	{
		return static_cast<Ogre::Item const&>(get_object(i));
	}

	mesh::mesh() noexcept
	{
		static_assert(sizeof(mesh::storage_mesh) == sizeof(Ogre::MeshPtr) && alignof(mesh) == alignof(Ogre::MeshPtr));

		new(storage_mesh) Ogre::MeshPtr;
	}

	mesh::mesh(mesh&& other) noexcept
	{
		new(storage_mesh) Ogre::MeshPtr(get_mesh_ptr(std::move(other)));
	}

	mesh& mesh::operator=(mesh&& other) noexcept
	{
		if (this != &other)
		{
			destroy_mesh();
			get_mesh_ptr(*this) = get_mesh_ptr(std::move(other));
		}
		return *this;
	}

	mesh::~mesh()
	{
		destroy_mesh();
		get_mesh_ptr(*this).~SharedPtr();
	}

	void mesh::destroy_mesh() noexcept
	{
		auto& mesh_ptr = get_mesh_ptr(*this);
		if (mesh_ptr != nullptr)
		{
			auto& mesh_manager = Ogre::MeshManager::getSingleton();
			mesh_manager.remove(mesh_ptr);
		}
	}

	std::string const& mesh::get_mesh_name() const noexcept
	{
		return get_mesh_ptr(*this)->getName();
	}

	namespace
	{
		struct render_vertex
		{
			math::point3f position;
			math::vector3f normal;
			math::point2f uv;

			[[nodiscard]] static Ogre::VertexElement2Vec get_vertex_buffer_elements()
			{
				return {
					Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_POSITION)
					, Ogre::VertexElement2(Ogre::VET_FLOAT3, Ogre::VES_NORMAL)
					, Ogre::VertexElement2(Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES)
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
				auto const face_vertex_count = face.get_vertex_count();
				assert(face_vertex_count >= 3); // I've had issues at some point, better make sure
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
				auto const vertices = face.get_vertices();
				auto const face_vertex_count = face.get_vertex_count();
				auto const normal = face.get_normal();

				auto const base_index = static_cast<Ogre::uint16>(std::distance(vertex_mem_begin, vertex_it));

				// push vertices
				for (auto const vertex : vertices)
				{
					new(vertex_it++) render_vertex{ vertex.get_position(), normal, vertex.get_uv() };
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

		void set_mesh_bounds(Ogre::Mesh& render_mesh, math::aabb const& mesh_bounds)
		{
			math::point3f const bounds_center = mesh_bounds.position;
			math::vector3f const bounds_half_size = mesh_bounds.half_size;
			render_mesh._setBounds(Ogre::Aabb(to_ogre_vector(bounds_center), to_ogre_vector(bounds_half_size)), false);
			render_mesh._setBoundingSphereRadius(bounds_half_size.norm());
		}

		[[nodiscard]] Ogre::MeshPtr make_mesh(std::string const& name, mesh_definition const& mesh_def)
		{
			auto& mesh_manager = Ogre::MeshManager::getSingleton();

			Ogre::MeshPtr render_mesh = mesh_manager.createManual(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			Ogre::SubMesh* render_submesh = render_mesh->createSubMesh();
			auto const vao = make_render_vao(mesh_def);
			render_submesh->mVao[Ogre::VpNormal].push_back(vao);
			render_submesh->mVao[Ogre::VpShadow].push_back(vao);

			set_mesh_bounds(*render_mesh, mesh_def.get_bounds());

			return render_mesh;
		}
	}

	void mesh::reload_mesh(mesh_definition const& mesh_def)
	{
		Ogre::MeshPtr& ptr = get_mesh_ptr(*this);

		auto const name = ptr->getName(); // copy

		destroy_mesh();

		ptr = make_mesh(name, mesh_def);
	}

	material_handle_t item_ref::get_material() const
	{
		Ogre::Item& item = get_item(*this);
		Ogre::SubItem const* sub_item = item.getSubItem(0);
		Ogre::HlmsDatablock* datablock = sub_item->getDatablock();
		return to_material_handle(datablock->getName());
	}

	void item_ref::set_material(material_handle_t const& mat)
	{
		Ogre::Item& item = get_item(*this);
		Ogre::SubItem* sub_item = item.getSubItem(0);

		auto& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager* const hlms_manager = root.getHlmsManager();

		Ogre::IdString const id_string = to_id_string(mat);
		if (id_string == Ogre::IdString())
		{
			sub_item->setDatablock(hlms_manager->getDefaultDatablock());
		}
		else
		{
			Ogre::HlmsDatablock* datablock = hlms_manager->getDatablockNoDefault(id_string);
			if (datablock == nullptr)
			{
				throw std::invalid_argument(std::format("Invalid material '{}'", mat.to_debug_string()));
			}

			sub_item->setDatablock(datablock);
		}
	}

	template<>
	item_ref ref_cast<item_ref>(object_ref ref)
	{
		return make_item_ref(static_cast<Ogre::Item&>(get_object(ref)));
	}

	item_cref::item_cref(item_ref rhs) noexcept
	{
		init_object_cref(*this, get_object(rhs));
	}

	material_handle_t item_cref::get_material() const
	{
		Ogre::Item const& item = get_item(*this);
		Ogre::SubItem const& sub_item = *item.getSubItem(0);
		Ogre::HlmsDatablock const& datablock = *sub_item.getDatablock();
		return to_material_handle(datablock.getName());
	}

	template<>
	item_cref ref_cast<item_cref>(object_cref ref)
	{
		return make_item_cref(static_cast<Ogre::Item const&>(get_object(ref)));
	}

	mesh create_mesh(std::string const& name, mesh_definition const& mesh_def)
	{		
		Ogre::MeshPtr render_mesh = make_mesh(name, mesh_def);
		
		mesh m;
		init_mesh(m, std::move(render_mesh));
		return m;
	}

	void add_item(node_ref owner, mesh const& m)
	{
		Ogre::SceneNode& owner_node = get_scene_node(owner);
		Ogre::SceneManager& scene_manager = *owner_node.getCreator();
		Ogre::Item* const item = scene_manager.createItem(get_mesh_ptr(m), Ogre::SCENE_DYNAMIC);
		owner_node.attachObject(item);
	}
}
