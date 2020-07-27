#include "graphics/module.h"

#include "graphics/mesh_definition.h"

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
#include "Ogre/ConfigOptionMap.h"
#include "Ogre/RenderSystem.h"
#include "Ogre/Window.h"

#include "math/vector3.h"

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

		math::plane const pyramid_planes[] = {
			{{0, -1, 0}, 0.5},
			{{sqrt_half, sqrt_half, 0}, 0.5},
			{{-sqrt_half, sqrt_half, 0}, 0.5},
			{{0, sqrt_half, sqrt_half}, 0.5},
			{{0, sqrt_half, -sqrt_half}, 0.5},
		};

		struct MeshNode
		{
			mesh_definition mesh;
			Ogre::MeshPtr render_mesh;
			Ogre::SceneNode* node;
		};

		bool get_vsync(ot::ogre::config_option_map const& config)
		{
			bool vsync;
			return ot::ogre::render_system::get_vsync(config, vsync) ? vsync : false;
		}

		bool get_hardware_gamma_conversion(ot::ogre::config_option_map const& config)
		{
			bool gamma;
			return ot::ogre::render_system::get_hardware_gamma_conversion(config, gamma) ? gamma : false;
		}
	}

	struct module::impl
	{
		Ogre::SceneManager* scene_manager;
		Ogre::Camera* main_camera;
		Ogre::CompositorWorkspace* main_workspace;
		Ogre::Window* render_window;
		window_id render_window_id;

		std::vector<MeshNode> scene_nodes;

		Ogre::MeshPtr selection_mesh;

		std::unique_ptr<Ogre::v1::OverlaySystem> overlay_system;
		Ogre::v1::Overlay* debug_overlay;
		std::optional<shadowed_text> debug_text;

		~impl()
		{
			cleanup();
		}

		void initialize(window_parameters window_params, size_t number_threads)
		{
			auto& root = Ogre::Root::getSingleton();
			auto const& config = root.getRenderSystem()->getConfigOptions();

			auto const misc_params = ot::ogre::render_window_misc_params{}
				.set_external_window_handle(window_params.window_handle)
				.set_hardware_gamma_conversion(get_hardware_gamma_conversion(config))
				.set_vsync(get_vsync(config))
				.params;
			render_window = root.createRenderWindow(window_params.window_handle, window_params.width, window_params.height, window_params.fullscreen, &misc_params);
			render_window_id = window_params.event_id;

			scene_manager = root.createSceneManager(Ogre::ST_GENERIC, number_threads);

			main_camera = scene_manager->createCamera("Main Camera");
			main_camera->setPosition(Ogre::Vector3(0, 2.5, -7.5));
			main_camera->lookAt(Ogre::Vector3(0, 0, 0));
			main_camera->setNearClipDistance(0.2f);
			main_camera->setFarClipDistance(1000.0f);
			main_camera->setAutoAspectRatio(true);

			Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
			compositor_manager->createBasicWorkspaceDef("Test Workspace", k_background_color);
			main_workspace = compositor_manager->addWorkspace(scene_manager, render_window->getTexture(), main_camera, "Test Workspace", true /*enabled*/);

			overlay_system = std::make_unique<Ogre::v1::OverlaySystem>();
			scene_manager->addRenderQueueListener(overlay_system.get());
			scene_manager->getRenderQueue()->setSortRenderQueue(Ogre::v1::OverlayManager::getSingleton().mDefaultRenderQueueId, Ogre::RenderQueue::StableSort);

			scene_manager->setShadowDirectionalLightExtrusionDistance(500.0f);
			scene_manager->setShadowFarDistance(500.0f);
		}

		void cleanup()
		{
			for (auto const& scene_node : scene_nodes)
			{
				scene_node.node->removeAndDestroyAllChildren();
			}

			if (scene_manager != nullptr)
			{
				if (overlay_system != nullptr)
				{
					scene_manager->removeRenderQueueListener(overlay_system.get());
				}
				Ogre::Root::getSingleton().destroySceneManager(scene_manager);
			}

			overlay_system.reset();
			main_workspace = nullptr;
			main_camera = nullptr;			
			scene_manager = nullptr;
		}

		void on_window_events(std::span<window_event const> events)
		{
			for (ot::graphics::window_event const& event : events)
			{
				if (event.id != render_window_id)
				{
					continue;
				}

				std::visit([this](auto e)
				{
					if constexpr (std::is_same_v<decltype(e), window_event::moved>)
						render_window->windowMovedOrResized();
					else if constexpr (std::is_same_v<decltype(e), window_event::resized>)
						render_window->windowMovedOrResized();
					else if constexpr (std::is_same_v<decltype(e), window_event::focus_gained>)
						render_window->setFocused(true);
					else if constexpr (std::is_same_v<decltype(e), window_event::focus_lost>)
						render_window->setFocused(false);
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
			if (render_window->isVisible())
				return Ogre::Root::getSingleton().renderOneFrame();

			return true;
		}

		void setup_scene()
		{
			Ogre::SceneNode* root_node = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC);

			auto const cube_mesh = mesh_definition::make_from_planes(cube_planes);

			///  Meshes
			scene_nodes = { 
				make_mesh_node(cube_planes, "CustomCube", Ogre::Vector3(-2.5f, 0.f, 0.f)),
				make_mesh_node(octagon_planes, "CustomOctagon", Ogre::Vector3(0.f, 0.f, 0.f)),
				make_mesh_node(pyramid_planes, "CustomPyramid", Ogre::Vector3(2.5f, 0.f, 0.f)),
			};

			selection_mesh = make_wireframe_mesh("SelectionWire", cube_mesh);
			auto const child_node = scene_nodes[0].node->createChildSceneNode();
			child_node->attachObject(scene_manager->createItem(selection_mesh));

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

		MeshNode make_mesh_node(std::span<math::plane const> planes, ogre::string const& name, Ogre::Vector3 const& position)
		{
			auto mesh = mesh_definition::make_from_planes(planes);
			auto render_mesh = make_static_mesh(name, mesh);

			Ogre::SceneNode* const root_node = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC);
			Ogre::Item* const item = scene_manager->createItem(render_mesh, Ogre::SCENE_DYNAMIC);

			Ogre::SceneNode* const node = root_node->createChildSceneNode(Ogre::SCENE_DYNAMIC, position);
			node->attachObject(item);

			return { std::move(mesh), std::move(render_mesh), node };
		}
	};

	module::module()
		: pimpl(new impl)
	{

	}

	module::~module() = default;

	void module::initialize(window_parameters const& window_params, size_t number_threads)
	{
		pimpl->initialize(window_params, number_threads);
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
