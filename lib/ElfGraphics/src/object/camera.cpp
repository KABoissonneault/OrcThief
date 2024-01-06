#include "camera.h"

#include "object.h"

#include "Ogre/Ray.h"
#include "Ogre/Camera.h"
#include "Ogre/SceneManager.h"

#include "ogre_conversion.h"
#include "ogre_boost_conversion.h"

#include <utility>

namespace ot::egfx
{
	namespace detail
	{
		camera_cref make_camera_cref(void const* pimpl) noexcept
		{
			camera_cref o;
			init_object_cref(o, pimpl);
			return o;
		}

		camera_ref make_camera_ref(void* pimpl) noexcept
		{
			camera_ref o;
			init_object_ref(o, pimpl);
			return o;
		}

		camera make_camera(void* pimpl) noexcept
		{
			camera o;
			o.pimpl = pimpl;
			return o;
		}

		template<typename Derived>
		math::ray camera_const_impl<Derived>::get_world_ray(float x_norm, float y_norm) const
		{
			auto const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_ray(camera.getCameraToViewportRay(static_cast<Ogre::Real>(x_norm), static_cast<Ogre::Real>(y_norm)));
		}

		template<typename Derived>
		math::point3f camera_const_impl<Derived>::get_position() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_point(camera.getPosition());
		}

		template<typename Derived>
		math::vector3f camera_const_impl<Derived>::get_direction() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_vector(camera.getDirection());
		}

		template<typename Derived>
		math::quaternion camera_const_impl<Derived>::get_rotation() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_quaternion(camera.getOrientation());
		}

		template<typename Derived>
		float camera_const_impl<Derived>::get_rad_fov_x() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return camera.getFOVy().valueRadians() * camera.getAspectRatio();
		}

		template<typename Derived>
		float camera_const_impl<Derived>::get_rad_fov_y() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return camera.getFOVy().valueRadians();
		}

		template<typename Derived>
		float camera_const_impl<Derived>::get_aspect_ratio() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return camera.getAspectRatio();
		}

		template<typename Derived>
		math::transform_matrix camera_const_impl<Derived>::get_transformation() const
		{
			return math::transform_matrix::from_components(vector_from_origin(get_position()), get_rotation());
		}

		template<typename Derived>
		math::transform_matrix camera_const_impl<Derived>::get_projection() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_matrix(camera.getProjectionMatrix());
		}

		template<typename Derived>
		math::transform_matrix camera_const_impl<Derived>::get_view_matrix() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_matrix(camera.getViewMatrix());
		}

		template<typename Derived>
		float camera_const_impl<Derived>::get_z_near() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return camera.getNearClipDistance();
		}
		
		template<typename Derived>
		float camera_const_impl<Derived>::get_z_far() const
		{
			Ogre::Camera const& camera = get_camera(static_cast<derived const&>(*this));
			return camera.getFarClipDistance();
		}

		template class camera_const_impl<camera_cref>;
		template class camera_const_impl<camera_ref>;
		template class camera_const_impl<camera>;
	}

	camera_cref make_camera_cref(Ogre::Camera const& object) noexcept
	{
		return detail::make_camera_cref(&object);
	}

	camera_ref make_camera_ref(Ogre::Camera& object) noexcept
	{
		return detail::make_camera_ref(&object);
	}

	camera make_camera(Ogre::Camera& object) noexcept
	{
		return detail::make_camera(&object);
	}

	Ogre::Camera const& get_camera(camera_cref r) noexcept
	{
		return static_cast<Ogre::Camera const&>(get_object(r));
	}

	Ogre::Camera& get_camera(camera_ref r) noexcept
	{
		return static_cast<Ogre::Camera&>(get_object(r));
	}

	camera_cref::camera_cref(camera_ref rhs) noexcept
	{
		init_object_cref(*this, get_object(rhs));
	}

	template<>
	camera_cref ref_cast<camera_cref>(object_cref ref)
	{
		return make_camera_cref(static_cast<Ogre::Camera const&>(get_object(ref)));
	}

	void camera_ref::set_position(math::point3f position) const
	{
		get_camera(*this).setPosition(to_ogre_vector(position));
	}

	void camera_ref::move(math::vector3f displacement) const
	{
		get_camera(*this).moveRelative(to_ogre_vector(displacement));
	}

	void camera_ref::local_pitch(float rad) const
	{
		get_camera(*this).pitch(Ogre::Radian(rad));
	}

	void camera_ref::world_yaw(float rad) const
	{
		// We keep "fixed yaw" to true, which is set to +Y
		get_camera(*this).yaw(Ogre::Radian(rad));
	}

	void camera_ref::rotate(math::quaternion rotation) const
	{
		get_camera(*this).rotate(to_ogre_quaternion(rotation));
	}

	void camera_ref::look_at(math::point3f position) const
	{
		get_camera(*this).lookAt(to_ogre_vector(position));
	}

	template<>
	camera_ref ref_cast<camera_ref>(object_ref ref)
	{
		return make_camera_ref(static_cast<Ogre::Camera&>(get_object(ref)));
	}

	camera::camera() noexcept
		: pimpl(nullptr)
	{

	}
	camera::camera(camera&& other) noexcept
		: pimpl(std::exchange(other.pimpl, nullptr))
	{

	}
	camera& camera::operator=(camera&& other) noexcept
	{
		if (this != &other)
		{
			destroy_object();
			pimpl = std::exchange(other.pimpl, nullptr);
		}
		return *this;
	}

	camera::~camera()
	{
		destroy_object();
	}

	void camera::destroy_object()
	{
		if (pimpl == nullptr)
			return;

		Ogre::Camera& c = get_camera(*this);
		Ogre::SceneNode* const node = c.getParentSceneNode();
		if (node != nullptr)
		{
			node->detachObject(&c);
		}

		Ogre::SceneManager* const scene_manager = c.getSceneManager();		
		scene_manager->destroyCamera(&c);

		pimpl = nullptr;
	}

	void camera::set_position(math::point3f position)
	{
		static_cast<camera_ref>(*this).set_position(position);
	}

	void camera::move(math::vector3f displacement)
	{
		static_cast<camera_ref>(*this).move(displacement);
	}

	void camera::local_pitch(float rad)
	{
		static_cast<camera_ref>(*this).local_pitch(rad);
	}

	void camera::world_yaw(float rad)
	{
		static_cast<camera_ref>(*this).world_yaw(rad);
	}

	void camera::rotate(math::quaternion rotation)
	{
		static_cast<camera_ref>(*this).rotate(rotation);
	}

	void camera::look_at(math::point3f position)
	{
		static_cast<camera_ref>(*this).look_at(position);
	}
}
