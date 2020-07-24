#include "graphics/module.h"

#include "Ogre/Compositor/CompositorManager2.h"
#include "Ogre/Item.h"
#include "Ogre/Camera.h"
#include "Ogre/Components/Overlay.h"
#include "Ogre/Components/Overlay/Container.h"
#include "Ogre/Components/Overlay/Manager.h"
#include "Ogre/Components/Overlay/TextAreaElement.h"
#include "Ogre/Components/Overlay/System.h"
#include "Ogre/Mesh2.h"
#include "Ogre/SubMesh2.h"

#include "math/vector3.h"
#include "math/mesh.h"

#include "shadowed_text.h"
#include "static_mesh.h"

#include <optional>
#include <new>

const Ogre::ColourValue k_background_color(.8f, 0.4f, 0.4f);

namespace ot::graphics
{
	namespace
	{
		math::plane const cube_planes[6] = {
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		};

		Ogre::MeshPtr make_cube_mesh(ogre::string const& name)
		{
			return make_static_mesh(name, math::mesh::make_from_planes(cube_planes));
		}

		auto const sqrt_half = 0.70710678118654752440084436210485;
		math::plane const octagon_planes[] = {
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{1, 0, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, 0, 1}, 0.5},
			{{0, 0, -1}, 0.5},
			{{sqrt_half, 0, sqrt_half}, 0.5},
			{{sqrt_half, 0, -sqrt_half}, 0.5},
			{{-sqrt_half, 0, sqrt_half}, 0.5},
			{{-sqrt_half, 0, -sqrt_half}, 0.5},
		};

		Ogre::MeshPtr make_octagon_prism(ogre::string const& name)
		{
			return make_static_mesh(name, math::mesh::make_from_planes(octagon_planes));
		}

		math::plane const pyramid_planes[] = {
			{{0, -1, 0}, 0.5},
			{{sqrt_half, sqrt_half, 0}, 0.5},
			{{-sqrt_half, sqrt_half, 0}, 0.5},
			{{0, sqrt_half, sqrt_half}, 0.5},
			{{0, sqrt_half, -sqrt_half}, 0.5},
		};

		Ogre::MeshPtr make_pyramid(ogre::string const& name)
		{
			return make_static_mesh(name, math::mesh::make_from_planes(pyramid_planes));
		}

		struct MeshNode
		{
			Ogre::SceneNode* node;
			Ogre::MeshPtr mesh;
		};

		MeshNode make_mesh_node(Ogre::SceneManager* scene_manager, Ogre::MeshPtr mesh, Ogre::Vector3 const& position)
		{
			Ogre::SceneNode* const root_node = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC);
			Ogre::Item* const item = scene_manager->createItem(mesh, Ogre::SCENE_DYNAMIC);

			Ogre::SceneNode* const node = root_node->createChildSceneNode(Ogre::SCENE_DYNAMIC, position);
			node->attachObject(item);

			return { node, std::move(mesh) };
		}
	}

	struct module::impl
	{
		Ogre::SceneManager* scene_manager;
		Ogre::Camera* main_camera;
		Ogre::CompositorWorkspace* main_workspace;
		ot::graphics::window main_window;

		std::vector<MeshNode> scene_nodes;

		std::unique_ptr<Ogre::v1::OverlaySystem> overlay_system;
		Ogre::v1::Overlay* debug_overlay;
		std::optional<shadowed_text> debug_text;

		~impl()
		{
			cleanup();
		}

		void initialize(char const* window_title, size_t number_threads)
		{
			auto& root = Ogre::Root::getSingleton();

			main_window = ot::graphics::window::create(window_title);
			scene_manager = root.createSceneManager(Ogre::ST_GENERIC, number_threads);
			
			main_camera = scene_manager->createCamera("Main Camera");
			main_camera->setPosition(Ogre::Vector3(0, 2.5, -7.5));
			main_camera->lookAt(Ogre::Vector3(0, 0, 0));
			main_camera->setNearClipDistance(0.2f);
			main_camera->setFarClipDistance(1000.0f);
			main_camera->setAutoAspectRatio(true);

			Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
			compositor_manager->createBasicWorkspaceDef("Test Workspace", k_background_color);
			main_workspace = compositor_manager->addWorkspace(scene_manager, main_window.get_render_texture(), main_camera, "Test Workspace", true /*enabled*/);

			overlay_system = std::make_unique<Ogre::v1::OverlaySystem>();
			scene_manager->addRenderQueueListener(overlay_system.get());
			scene_manager->getRenderQueue()->setSortRenderQueue(Ogre::v1::OverlayManager::getSingleton().mDefaultRenderQueueId, Ogre::RenderQueue::StableSort);

			scene_manager->setShadowDirectionalLightExtrusionDistance(500.0f);
			scene_manager->setShadowFarDistance(500.0f);
		}

		void cleanup()
		{
			if (scene_manager != nullptr)
			{
				if (overlay_system != nullptr)
				{
					scene_manager->removeRenderQueueListener(overlay_system.get());
				}
			}

			overlay_system.reset();
			main_workspace = nullptr;
			main_camera = nullptr;
			scene_manager = nullptr;
			main_window.cleanup();
		}

		void on_window_events(std::span<window_event const> events)
		{
			for (ot::graphics::window_event const& event : events)
			{
				if (event.id != main_window.get_window_id())
				{
					continue;
				}

				std::visit([this](auto e)
				{
					if constexpr (std::is_same_v<decltype(e), window_event::moved>)
						main_window.on_moved(e.new_x, e.new_y);
					else if constexpr (std::is_same_v<decltype(e), window_event::resized>)
						main_window.on_resized(e.new_width, e.new_height);
					else if constexpr (std::is_same_v<decltype(e), window_event::focus_gained>)
						main_window.on_focus_gained();
					else if constexpr (std::is_same_v<decltype(e), window_event::focus_lost>)
						main_window.on_focus_lost();
				}, event.data);
			}
		}

		void update(math::seconds dt)
		{
			Ogre::String s;
			s += "Camera Pos: {" + Ogre::StringConverter::toString(main_camera->getPosition()) + "}\n";
			s += "Camera Rot: {" + Ogre::StringConverter::toString(main_camera->getDirection()) + "}\n";
			debug_text->set_text(s);

			for (auto & mesh_node : scene_nodes)
			{
				mesh_node.node->rotate(Ogre::Vector3(0.f, 1.f, 0.f), Ogre::Radian(dt.count()));
			}
		}

		bool render()
		{
			if (main_window.is_visible())
				return Ogre::Root::getSingleton().renderOneFrame();

			return true;
		}

		void setup_scene()
		{
			Ogre::SceneNode* root_node = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC);

			///  Meshes
			scene_nodes = { 
				make_mesh_node(scene_manager, make_cube_mesh("CustomCube"), Ogre::Vector3(-2.5f, 0.f, 0.f)),
				make_mesh_node(scene_manager, make_octagon_prism("CustomOctagon"), Ogre::Vector3(0.f, 0.f, 0.f)),
				make_mesh_node(scene_manager, make_pyramid("CustomPyramid"), Ogre::Vector3(2.5f, 0.f, 0.f)),
			};

			/// Lights
			Ogre::Light* light = scene_manager->createLight();
			Ogre::SceneNode* light_node = root_node->createChildSceneNode(Ogre::SCENE_DYNAMIC, Ogre::Vector3(10.f, 10.f, -10.f));
			light_node->attachObject(light);
			light->setPowerScale(Ogre::Math::PI);
			light->setType(Ogre::Light::LT_DIRECTIONAL);
			light->setDirection(Ogre::Vector3(-1.f, -1.f, 1.f).normalisedCopy());

			/// Overlay
			auto & manager = Ogre::v1::OverlayManager::getSingleton();

			ogre::string const debug_name("debug_overlay");
			debug_overlay = manager.create(debug_name);

			debug_text.emplace(debug_overlay, debug_name);
			debug_text->set_font("DebugFont");
			debug_text->set_height(0.025f);

			debug_overlay->show();
		}
	};

	module::module()
		: pimpl(new impl)
	{

	}

	module::~module() = default;

	void module::initialize(char const* window_title, size_t number_threads)
	{
		pimpl->initialize(window_title, number_threads);
	}

	void module::on_window_events(std::span<window_event const> events)
	{
		pimpl->on_window_events(events);
	}

	void module::update(math::seconds dt)
	{
		pimpl->update(dt);
	}

	bool module::render()
	{
		return pimpl->render();
	}

	void module::setup_scene()
	{
		pimpl->setup_scene();
	}	
}
