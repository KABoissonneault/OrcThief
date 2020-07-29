#pragma once

#include "Ogre/Prerequisites.h"
#include "Ogre/Components/Overlay/System.h"

#include "math/unit/time.h"

#include <memory>

namespace ot::graphics
{
	class scene
	{
		Ogre::SceneManager* scene_manager;
		Ogre::Camera* main_camera;
		Ogre::CompositorWorkspace* main_workspace;
		std::unique_ptr<Ogre::v1::OverlaySystem> overlay_system;

	public:
		scene(size_t number_threads, Ogre::TextureGpu* render_window, ogre::string const& workspace_def);
		~scene();

		void update(math::seconds dt);
	};
}