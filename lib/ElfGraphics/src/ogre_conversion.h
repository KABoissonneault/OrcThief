#pragma once

#include "Ogre/Vector3.h"
#include "Ogre/Quaternion.h"
#include "Ogre/Ray.h"
#include "Ogre/ColourValue.h"
#include "Ogre/Matrix4.h"

#include "math/vector3.h"
#include "math/quaternion.h"
#include "math/ray.h"
#include "egfx/color.h"
#include "math/transform_matrix.h"

namespace ot::egfx
{
	[[nodiscard]] inline Ogre::Vector3 to_ogre_vector(math::point3f v) noexcept
	{
		return {
			v.x,
			v.y,
			v.z
		};
	}

	[[nodiscard]] inline Ogre::Vector3 to_ogre_vector(math::vector3f v) noexcept
	{
		return {
			v.x,
			v.y,
			v.z
		};
	}

	[[nodiscard]] inline math::point3f to_math_point(Ogre::Vector3 v) noexcept
	{
		return {
			v.x,
			v.y,
			v.z
		};
	}

	[[nodiscard]] inline math::vector3f to_math_vector(Ogre::Vector3 v) noexcept
	{
		return {
			v.x,
			v.y,
			v.z
		};
	}

	[[nodiscard]] inline math::quaternion to_math_quaternion(Ogre::Quaternion q) noexcept
	{
		return { q.w, q.x, q.y, q.z };
	}

	[[nodiscard]] inline Ogre::Quaternion to_ogre_quaternion(math::quaternion q) noexcept
	{
		return { q.w, q.x, q.y, q.z };
	}

	[[nodiscard]] inline math::ray to_math_ray(Ogre::Ray r) noexcept
	{
		return {
			to_math_point(r.getOrigin()),
			to_math_vector(r.getDirection())
		};
	}

	[[nodiscard]] inline Ogre::Ray to_ogre_ray(math::ray r) noexcept
	{
		return {
			to_ogre_vector(r.origin),
			to_ogre_vector(r.direction)
		};
	}

	[[nodiscard]] inline color to_graphics_color(Ogre::ColourValue c) noexcept
	{
		return {
			c.r,
			c.g,
			c.b,
			c.a
		};
	}

	[[nodiscard]] inline Ogre::ColourValue to_ogre_colour(color c) noexcept
	{
		return Ogre::ColourValue {
			c.r,
			c.g,
			c.b,
			c.a
		};
	}

	[[nodiscard]] inline math::transform_matrix to_math_matrix(Ogre::Matrix4 const& m) noexcept
	{
		// assume both are column-major
		static_assert(sizeof(math::transform_matrix) == sizeof(Ogre::Matrix4));
		math::transform_matrix ret;
		memcpy(&ret, &m[0][0], sizeof(float) * 16);
		return ret;
	}

	[[nodiscard]] inline Ogre::Matrix4 to_ogre_matrix(math::transform_matrix const& m) noexcept
	{
		static_assert(sizeof(math::transform_matrix) == 16 * sizeof(Ogre::Real));
		return Ogre::Matrix4(m.elements);
	}
}
