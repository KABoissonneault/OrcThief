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
		init_object(scene_root, scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC));

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
		scene_root = {};

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

	}

	camera& get_camera(scene& s)
	{
		return get_impl(s).get_camera();
	}

	node::object& get_root_node(scene& s)
	{
		return get_impl(s).get_root_node();
	}
}

template struct ot::fwd_delete<ot::graphics::scene_impl>;
