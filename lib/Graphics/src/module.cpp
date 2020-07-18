#include "graphics/module.h"

#include "Ogre/Compositor/CompositorManager2.h"
#include "Ogre/Item.h"
#include "Ogre/Camera.h"
#include "Ogre/Components/Overlay.h"
#include "Ogre/Components/Overlay/Container.h"
#include "Ogre/Components/Overlay/Manager.h"
#include "Ogre/Components/Overlay/TextAreaElement.h"
#include "Ogre/Components/Overlay/System.h"

#include "math/vector3.h"

#include "shadowed_text.h"

#include <optional>

const Ogre::ColourValue k_background_color(.8f, 0.4f, 0.4f);

namespace ot::graphics
{
	struct module::impl
	{
		Ogre::SceneManager* scene_manager;
		Ogre::Camera* main_camera;
		Ogre::CompositorWorkspace* main_workspace;
		ot::graphics::window main_window;

		Ogre::SceneNode* cube_node;

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

			Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
			compositor_manager->createBasicWorkspaceDef("Test Workspace", k_background_color);
			main_workspace = compositor_manager->addWorkspace(scene_manager, main_window.get_render_texture(), main_camera, "Test Workspace", true /*enabled*/);

			overlay_system = std::make_unique<Ogre::v1::OverlaySystem>();
			scene_manager->addRenderQueueListener(overlay_system.get());
			scene_manager->getRenderQueue()->setSortRenderQueue(Ogre::v1::OverlayManager::getSingleton().mDefaultRenderQueueId, Ogre::RenderQueue::StableSort);
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
		}

		bool render()
		{
			if (main_window.is_visible())
				return Ogre::Root::getSingleton().renderOneFrame();

			return true;
		}

		void setup_scene()
		{
			Ogre::SceneNode* dynamic_root = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC);
			cube_node = dynamic_root->createChildSceneNode(Ogre::SCENE_DYNAMIC);
			cube_node->setPosition(Ogre::Vector3(-5.0f, 0.f, 0.f));

			Ogre::Item* cube = scene_manager->createItem("Cube_d.mesh", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, Ogre::SCENE_DYNAMIC);
			cube_node->attachObject(cube);

			// Position it at 500 in Z direction
			main_camera->setPosition(Ogre::Vector3(0, 5, 15));
			// Look back along -Z
			main_camera->lookAt(Ogre::Vector3(0, 0, 0));
			main_camera->setNearClipDistance(0.2f);
			main_camera->setFarClipDistance(1000.0f);
			main_camera->setAutoAspectRatio(true);

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
