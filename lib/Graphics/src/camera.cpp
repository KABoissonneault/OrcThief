#include "camera.h"

namespace ot::graphics
{
	void set_position(camera& c, math::vector3d position)
	{
		c->setPosition(static_cast<Ogre::Real>(position.x), static_cast<Ogre::Real>(position.y), static_cast<Ogre::Real>(position.z));
	}

	void look_at(camera& c, math::vector3d position)
	{
		c->lookAt(static_cast<Ogre::Real>(position.x), static_cast<Ogre::Real>(position.y), static_cast<Ogre::Real>(position.z));
	}
}
