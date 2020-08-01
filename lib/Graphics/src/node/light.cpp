#include "light.h"

namespace ot::graphics::node
{
	directional_light make_directional_light(Ogre::SceneNode* light_node)
	{
		node::directional_light light;
		init_object(light, light_node);
		return light;
	}
}