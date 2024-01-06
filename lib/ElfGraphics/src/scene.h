#pragma once

#include "egfx/scene.h"
#include "egfx/object/camera.h"

#include "Ogre/Prerequisites.h"

#include "math/unit/time.h"

namespace ot::egfx
{
	class scene_impl
	{
		Ogre::SceneManager* scene_manager;
		camera main_camera;
		Ogre::CompositorWorkspace* main_workspace;

	public:
		scene_impl(size_t number_threads, Ogre::TextureGpu* render_window, ogre::string const& workspace_def);
		~scene_impl();

		Ogre::SceneManager& get_scene_manager() noexcept { return *scene_manager; }
		
		camera_ref get_camera() noexcept { return main_camera; }
		camera_cref get_camera() const noexcept { return main_camera; }
		node_ref get_root_node() noexcept;

		void update(math::seconds dt);

		void set_ambiant_light(color upper_hemisphere, color lower_hemisphere, math::vector3f direction);

		std::vector<object_id> raycast_objects(math::ray r) const;
	};

	void init_scene(scene& s, uptr<scene_impl, fwd_delete<scene_impl>> p);
	scene_impl& get_impl(scene& s);
}