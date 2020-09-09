#pragma once

#include "egfx/object/camera.fwd.h"

#include "math/vector3.h"

#include "math/ray.h"

namespace ot::egfx::object
{
	namespace detail
	{
		camera_cref make_camera_cref(void const*) noexcept;
		camera_ref make_camera_ref(void*) noexcept;
		camera make_camera(void*) noexcept;
		void const* get_camera_impl(camera_cref) noexcept;
		void* get_camera_impl(camera_ref) noexcept;

		template<typename Derived>
		class camera_const_impl
		{
			using derived = Derived;

		public:
			// Projects a raycast from the viewport of the main camera to the world
			// 
			//   x_norm: distance from the left of the screen in normalized coordinates [0, 1]
			//   y_norm: distance from the top of the screen in normalizes coordinates [0, 1]
			//
			// Returns: ray from viewport position to the world
			[[nodiscard]] math::ray get_world_ray(double x_norm, double y_norm) const;

			// Gets the local position of the camera
			[[nodiscard]] math::point3d get_position() const;

			// Get the radians of the field of view on the horizontal axis
			[[nodiscard]] double get_rad_fov_x() const;
			// Get the radians of the field of view on the vertical axis
			[[nodiscard]] double get_rad_fov_y() const;
		};
	}

	class camera_cref : public detail::camera_const_impl<camera_cref>
	{
		void const* pimpl;

		friend camera_cref detail::make_camera_cref(void const*) noexcept;
		friend void const* detail::get_camera_impl(camera_cref) noexcept;

		friend class detail::camera_const_impl<camera_cref>;

	public:

	};

	class camera_ref : public detail::camera_const_impl<camera_ref>
	{
		void* pimpl;

		friend camera_ref detail::make_camera_ref(void*) noexcept;
		friend void* detail::get_camera_impl(camera_ref) noexcept;

		friend class detail::camera_const_impl<camera_cref>;

	public:
		operator camera_cref() const noexcept { return detail::make_camera_cref(pimpl); }

		// Sets the local position of the camera
		void set_position(math::point3d position) const;
		// Displaces the local position of the camera
		void move(math::vector3d displacement) const;
		// Rotate the camera around its local pitch axis
		void local_pitch(double rad) const;
		// Rotate the camera around the world yaw axis (ie: +Y)
		void world_yaw(double rad) const;
		// Rotate the camera around its local axes
		void rotate(math::quaternion rotation) const;
		// Sets the orientation of the camera so that the center of the viewport faces the position
		void look_at(math::point3d position) const;
	};

	class camera : public detail::camera_const_impl<camera>
	{
		void* pimpl;

		friend camera detail::make_camera(void*) noexcept;

		friend class detail::camera_const_impl<camera_cref>;

		void destroy_object();

	public:
		camera() noexcept;
		camera(camera const&) = delete;
		camera(camera &&) noexcept;
		camera& operator=(camera const&) = delete;
		camera& operator=(camera &&) noexcept;
		~camera();

		operator camera_cref() const noexcept { return detail::make_camera_cref(pimpl); }
		operator camera_ref() noexcept { return detail::make_camera_ref(pimpl); }

		// Sets local position of the camera
		void set_position(math::point3d position);
		// Displaces the local position of the camera
		void move(math::vector3d displacement);
		// Rotate the camera around its local pitch axis
		void local_pitch(double rad);
		// Rotate the camera around the world yaw axis (ie: +Y)
		void world_yaw(double rad);
		// Rotate the camera around its local axes
		void rotate(math::quaternion rotation);
		// Sets the orientation of the camera so that the center of the viewport faces the position
		void look_at(math::point3d position);
	};

	extern template class detail::camera_const_impl<camera_cref>;
	extern template class detail::camera_const_impl<camera_ref>;
	extern template class detail::camera_const_impl<camera>;
}
