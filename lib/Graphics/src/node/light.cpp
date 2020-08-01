#include "light.h"

#include "scene.h"

#include "Ogre/SceneManager.h"

namespace ot::graphics::node
{
	directional_light create_directional_light(scene& s, object& parent)
	{
		Ogre::SceneManager& scene_manager = get_impl(s).get_scene_manager();
		Ogre::Light* light_object = scene_manager.createLight();
		light_object->setPowerScale(Ogre::Math::PI); // Apparently, "PBS" makes light weaker by a factor of PI when HDR is not used
		light_object->setType(Ogre::Light::LT_DIRECTIONAL);
		Ogre::SceneNode* light_node = scene_manager.getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
		light_node->attachObject(light_object);

		node::directional_light light;
		init_object(light, light_node);
		return light;
	}
}