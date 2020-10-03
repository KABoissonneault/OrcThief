#pragma once

#include "egfx/object/camera.fwd.h"

#include "math/vector3.h"
#include "math/transform_matrix.h"

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
			[[nodiscard]] math::ray get_world_ray(float x_norm, float y_norm) const;

			// Gets the local position of the camera
			[[nodiscard]] math::point3f get_position() const;
			// Gets the direction of the camera
			[[nodiscard]] math::vector3f get_direction() const;
			// Gets the rotation of the camera
			[[nodiscard]] math::quaternion get_rotation() const;

			// Get the radians of the field of view on the horizontal axis
			[[nodiscard]] float get_rad_fov_x() const;
			// Get the radians of the field of view on the vertical axis
			[[nodiscard]] float get_rad_fov_y() const;
			// Get the ratio between the x FoV and the y FoV
			[[nodiscard]] float get_aspect_ratio() const;

			// Get the transformation of the camera
			[[nodiscard]] math::transform_matrix get_transformation() const;
			// Get the 4x4 matrix representing the camera projection
			// This is not actually the transformation matrix of the camera
			[[nodiscard]] math::transform_matrix get_projection() const;
			// Get the 4x4 matrix representing the camera view
			// This is not actually the transformation matrix of the camera
			[[nodiscard]] math::transform_matrix get_view_matrix() const;
			
			// Get the distance of the far plane along the Z axis
			[[nodiscard]] float get_z_near() const;
			// Get the distance of the near place along the Z axis
			[[nodiscard]] float get_z_far() const;
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
		void set_position(math::point3f position) const;
		// Displaces the local position of the camera
		void move(math::vector3f displacement) const;
		// Rotate the camera around its local pitch axis
		void local_pitch(float rad) const;
		// Rotate the camera around the world yaw axis (ie: +Y)
		void world_yaw(float rad) const;
		// Rotate the camera around its local axes
		void rotate(math::quaternion rotation) const;
		// Sets the orientation of the camera so that the center of the viewport faces the position
		void look_at(math::point3f position) const;
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
		void set_position(math::point3f position);
		// Displaces the local position of the camera
		void move(math::vector3f displacement);
		// Rotate the camera around its local pitch axis
		void local_pitch(float rad);
		// Rotate the camera around the world yaw axis (ie: +Y)
		void world_yaw(float rad);
		// Rotate the camera around its local axes
		void rotate(math::quaternion rotation);
		// Sets the orientation of the camera so that the center of the viewport faces the position
		void look_at(math::point3f position);
	};

	extern template class detail::camera_const_impl<camera_cref>;
	extern template class detail::camera_const_impl<camera_ref>;
	extern template class detail::camera_const_impl<camera>;
}
