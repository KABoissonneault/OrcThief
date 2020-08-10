#pragma once

#include "graphics/scene.h"
#include "graphics/node/object.h"
#include "camera.h"

#include "Ogre/Prerequisites.h"
#include "Ogre/Components/Overlay/System.h"

#include "math/unit/time.h"

#include <memory>

namespace ot::graphics
{
	class scene_impl
	{
		Ogre::SceneManager* scene_manager;
		node::object scene_root;
		camera main_camera;
		Ogre::CompositorWorkspace* main_workspace;

	public:
		scene_impl(size_t number_threads, Ogre::TextureGpu* render_window, ogre::string const& workspace_def);
		~scene_impl();

		Ogre::SceneManager& get_scene_manager() noexcept { return *scene_manager; }
		
		camera& get_camera() noexcept { return main_camera; }
		camera const& get_camera() const noexcept { return main_camera; }
		node::object& get_root_node() noexcept { return scene_root; }

		void update(math::seconds dt);

		std::vector<node::object_id> raycast_objects(math::ray r) const;
	};

	void init_scene(scene& s, uptr<scene_impl, fwd_delete<scene_impl>> p);
	scene_impl& get_impl(scene& s);
}