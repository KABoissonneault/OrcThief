#include "light.h"

#include "object.h"

#include "Ogre/SceneManager.h"
#include "Ogre/SceneNode.h"

namespace ot::graphics::node
{
	directional_light create_directional_light(object_ref parent)
	{
		Ogre::SceneManager& scene_manager = *get_scene_node(parent).getCreator();
		Ogre::Light* light_object = scene_manager.createLight();
		light_object->setPowerScale(Ogre::Math::PI); // Apparently, "PBS" makes light weaker by a factor of PI when HDR is not used
		light_object->setType(Ogre::Light::LT_DIRECTIONAL);
		Ogre::SceneNode* light_node = scene_manager.getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
		light_node->attachObject(light_object);

		directional_light light;
		light.ref = make_object_ref(*light_node);
		return light;
	}
}