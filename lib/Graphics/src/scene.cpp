#include "scene.h"

#include "node/static_mesh.h"
#include "node/light.h"

#include "core/fwd_delete.h"

#include "Ogre/Root.h"

#include "Ogre/Camera.h"
#include "Ogre/Compositor/CompositorManager2.h"
#include "Ogre/Components/Overlay/Manager.h"

namespace ot::graphics
{
	void init_scene(scene& s, uptr<scene_impl, fwd_delete<scene_impl>> p)
	{
		s.pimpl = std::move(p);
	}

	scene_impl& get_impl(scene& s)
	{
		return *s.pimpl;
	}

	scene_impl::scene_impl(size_t number_threads, Ogre::TextureGpu* render_texture, ogre::string const& workspace_def)
	{
		auto& root = Ogre::Root::getSingleton();

		scene_manager = root.createSceneManager(Ogre::ST_GENERIC, number_threads);

		main_camera = scene_manager->createCamera("Main Camera");
		auto const camera = main_camera.get();
		camera->setNearClipDistance(0.2f);
		camera->setFarClipDistance(1000.0f);
		camera->setAutoAspectRatio(true);

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		main_workspace = compositor_manager->addWorkspace(scene_manager, render_texture, main_camera.get(), workspace_def, true /*enabled*/);

		overlay_system = std::make_unique<Ogre::v1::OverlaySystem>();
		scene_manager->addRenderQueueListener(overlay_system.get());
		scene_manager->getRenderQueue()->setSortRenderQueue(Ogre::v1::OverlayManager::getSingleton().mDefaultRenderQueueId, Ogre::RenderQueue::StableSort);

		scene_manager->setShadowDirectionalLightExtrusionDistance(500.0f);
		scene_manager->setShadowFarDistance(500.0f);
	}

	scene_impl::~scene_impl()
	{
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

	void scene_impl::update(math::seconds dt)
	{
		/*
		Ogre::String s;
		s += "Camera Pos: {" + Ogre::StringConverter::toString(main_camera->getPosition()) + "}\n";
		s += "Camera Rot: {" + Ogre::StringConverter::toString(main_camera->getDirection()) + "}\n";
		debug_text->set_text(s);

		for (auto& mesh_node : scene_nodes)
		{
			mesh_node.node->rotate(Ogre::Vector3(0.f, 1.f, 0.f), Ogre::Radian(dt.count()));
		}
		*/
	}

	/*
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
		auto& manager = Ogre::v1::OverlayManager::getSingleton();

		ogre::string const debug_name("debug_overlay");
		debug_overlay = manager.create(debug_name);

		debug_text.emplace(debug_overlay, debug_name);
		debug_text->set_font("DebugFont");
		debug_text->set_height(0.025f);

		debug_overlay->show();
	}
	*/

	node::static_mesh make_static_mesh_node(scene& s, std::string const& name, mesh_definition const& mesh)
	{
		return node::make_static_mesh(get_impl(s).get_scene_manager(), name, mesh);
	}

	node::directional_light make_directional_light(scene& s)
	{
		auto& scene_manager = get_impl(s).get_scene_manager();
		Ogre::Light* light_object = scene_manager.createLight();
		light_object->setPowerScale(Ogre::Math::PI); // TODO: why
		light_object->setType(Ogre::Light::LT_DIRECTIONAL);
		Ogre::SceneNode* light_node = scene_manager.getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
		light_node->attachObject(light_object);		

		return node::make_directional_light(light_node);
	}

	camera& get_camera(scene& s)
	{
		return get_impl(s).get_camera();
	}
}

template struct ot::fwd_delete<ot::graphics::scene_impl>;
