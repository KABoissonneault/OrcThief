#include "camera.h"

#include "Ogre/Ray.h"
#include "Ogre/Camera.h"
#include "Ogre/SceneManager.h"

#include "ogre_conversion.h"

#include <utility>

namespace ot::egfx::object
{
	namespace detail
	{
		camera_cref make_camera_cref(void const* pimpl) noexcept
		{
			camera_cref o;
			o.pimpl = pimpl;
			return o;
		}

		camera_ref make_camera_ref(void* pimpl) noexcept
		{
			camera_ref o;
			o.pimpl = pimpl;
			return o;
		}

		camera make_camera(void* pimpl) noexcept
		{
			camera o;
			o.pimpl = pimpl;
			return o;
		}

		void const* get_camera_impl(camera_cref r) noexcept
		{
			return r.pimpl;
		}

		void* get_camera_impl(camera_ref r) noexcept
		{
			return r.pimpl;
		}

		template<typename Derived>
		[[nodiscard]] math::ray camera_const_impl<Derived>::get_world_ray(double x_norm, double y_norm) const
		{
			auto const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_ray(camera.getCameraToViewportRay(static_cast<Ogre::Real>(x_norm), static_cast<Ogre::Real>(y_norm)));
		}

		template<typename Derived>
		[[nodiscard]] math::point3d camera_const_impl<Derived>::get_position() const
		{
			auto const& camera = get_camera(static_cast<derived const&>(*this));
			return to_math_point(camera.getPosition());
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
		return *static_cast<Ogre::Camera const*>(detail::get_camera_impl(r));
	}

	Ogre::Camera& get_camera(camera_ref r) noexcept
	{
		return *static_cast<Ogre::Camera*>(detail::get_camera_impl(r));
	}

	void camera_ref::set_position(math::point3d position) const
	{
		get_camera(*this).setPosition(to_ogre_vector(position));
	}

	void camera_ref::look_at(math::point3d position) const
	{
		get_camera(*this).lookAt(to_ogre_vector(position));
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
		Ogre::SceneManager* const scene_manager = node->getCreator();

		node->detachObject(&c);
		scene_manager->destroyCamera(&c);
	}

	void camera::set_position(math::point3d position)
	{
		return static_cast<camera_ref>(*this).set_position(position);
	}

	void camera::look_at(math::point3d position)
	{
		return static_cast<camera_ref>(*this).look_at(position);
	}
}
