#pragma once

#include "Ogre/Vector3.h"
#include "Ogre/Ray.h"

#include "math/vector3.h"
#include "math/ray.h"

namespace ot::graphics
{
	inline Ogre::Vector3 to_ogre_vector(math::point3d v) noexcept
	{
		return {
			static_cast<Ogre::Real>(v.x),
			static_cast<Ogre::Real>(v.y),
			static_cast<Ogre::Real>(v.z)
		};
	}

	inline Ogre::Vector3 to_ogre_vector(math::vector3d v) noexcept
	{
		return {
			static_cast<Ogre::Real>(v.x),
			static_cast<Ogre::Real>(v.y),
			static_cast<Ogre::Real>(v.z)
		};
	}

	inline math::point3d to_math_point(Ogre::Vector3 v) noexcept
	{
		return {
			v.x,
			v.y,
			v.z
		};
	}

	inline math::vector3d to_math_vector(Ogre::Vector3 v) noexcept
	{
		return {
			v.x,
			v.y,
			v.z
		};
	}

	inline math::ray to_graphics_ray(Ogre::Ray r) noexcept
	{
		return {
			to_math_point(r.getOrigin()),
			to_math_vector(r.getDirection())
		};
	}

	inline Ogre::Ray to_ogre_ray(math::ray r) noexcept
	{
		return {
			to_ogre_vector(r.origin),
			to_ogre_vector(r.direction)
		};
	}
}
