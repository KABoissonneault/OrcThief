#include "camera.h"

#include "Ogre/Ray.h"

#include "ogre_conversion.h"

namespace ot::graphics
{
	void set_position(camera& c, math::point3d position)
	{
		c->setPosition(to_ogre_vector(position));
	}

	math::point3d get_position(camera const& c)
	{
		return to_math_point(c->getPosition());
	}

	void look_at(camera& c, math::point3d position)
	{
		c->lookAt(to_ogre_vector(position));
	}

	math::ray get_world_ray_from_viewport(camera const& c, double viewport_x, double viewport_y)
	{
		return to_graphics_ray(c->getCameraToViewportRay(static_cast<Ogre::Real>(viewport_x), static_cast<Ogre::Real>(viewport_y)));
	}
}
