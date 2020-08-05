#include "camera.h"

#include "Ogre/Ray.h"

#include "ogre_conversion.h"

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

	ray get_world_ray_from_viewport(camera const& c, double viewport_x, double viewport_y)
	{
		return to_graphics_ray(c->getCameraToViewportRay(static_cast<Ogre::Real>(viewport_x), static_cast<Ogre::Real>(viewport_y)));
	}
}
