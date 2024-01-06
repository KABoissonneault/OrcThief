#include "scene.h"

#include "node.h"
#include "ogre_conversion.h"
#include "object/camera.h"

#include "core/fwd_delete.h"

#include "Ogre/Root.h"
#include "Ogre/Camera.h"
#include "Ogre/Compositor/Manager2.h"

namespace ot::egfx
{
	void init_scene(scene& s, uptr<scene_impl, fwd_delete<scene_impl>> p)
	{
		s.pimpl = std::move(p);
	}

	scene_impl& get_impl(scene& s)
	{
		return *s.pimpl;
	}

	scene_impl const& get_impl(scene const& s)
	{
		return *s.pimpl;
	}

	scene_impl::scene_impl(size_t number_threads, Ogre::TextureGpu* render_texture, ogre::string const& workspace_def)
	{
		auto& root = Ogre::Root::getSingleton();

		scene_manager = root.createSceneManager(Ogre::ST_GENERIC, number_threads);

		main_camera = make_camera(*scene_manager->createCamera("Main Camera"));
		Ogre::Camera& camera = egfx::get_camera(main_camera);
		camera.setNearClipDistance(0.2f);
		camera.setFarClipDistance(1000.0f);
		camera.setAutoAspectRatio(true);

		Ogre::CompositorManager2* const compositor_manager = root.getCompositorManager2();
		main_workspace = compositor_manager->addWorkspace(scene_manager, render_texture, &camera, workspace_def, true /*enabled*/);

		scene_manager->setShadowDirectionalLightExtrusionDistance(500.0f);
		scene_manager->setShadowFarDistance(500.0f);
	}

	scene_impl::~scene_impl()
	{
		main_camera = {};

		if (scene_manager != nullptr)
		{
			Ogre::Root::getSingleton().destroySceneManager(scene_manager);
		}
	}

	node_ref scene_impl::get_root_node() noexcept
	{
		return make_node_ref(*scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC));
	}

	void scene_impl::update(math::seconds dt)
	{
		scene_manager->updateSceneGraph();
	}

	void scene_impl::set_ambiant_light(color upper_hemisphere, color lower_hemisphere, math::vector3f direction)
	{
		scene_manager->setAmbientLight(to_ogre_colour(upper_hemisphere), to_ogre_colour(lower_hemisphere), to_ogre_vector(direction));
	}

	std::vector<object_id> scene_impl::raycast_objects(math::ray r) const
	{
		Ogre::RaySceneQuery* const sceneQuery = scene_manager->createRayQuery(to_ogre_ray(r), Ogre::SceneManager::QUERY_ENTITY_DEFAULT_MASK);
		sceneQuery->setSortByDistance(true);

		Ogre::RaySceneQueryResult& result = sceneQuery->execute();
		
		std::vector<object_id> object_ids;
		object_ids.reserve(result.size());
		std::transform(result.begin(), result.end(), std::back_inserter(object_ids), [](Ogre::RaySceneQueryResultEntry const& r)
		{
			return static_cast<object_id>(r.movable->getId());
		});

		return object_ids;
	}
	
	camera_ref scene::get_camera() noexcept
	{
		return get_impl(*this).get_camera();
	}

	camera_cref scene::get_camera() const noexcept
	{
		return get_impl(*this).get_camera();
	}

	node_ref scene::get_root_node() noexcept
	{
		return get_impl(*this).get_root_node();
	}

	std::vector<object_id> scene::raycast_objects(math::ray r) const
	{
		return get_impl(*this).raycast_objects(r);
	}

	void scene::update(math::seconds dt)
	{
		get_impl(*this).update(dt);
	}

	void scene::set_ambiant_light(color upper_hemisphere, color lower_hemisphere, math::vector3f direction)
	{
		get_impl(*this).set_ambiant_light(upper_hemisphere, lower_hemisphere, direction);
	}
}

template struct ot::fwd_delete<ot::egfx::scene_impl>;
