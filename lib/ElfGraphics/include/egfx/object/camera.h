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

			[[nodiscard]] math::point3d get_position() const;
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

		void set_position(math::point3d position) const;
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

		void set_position(math::point3d position);
		void look_at(math::point3d position);
	};

	extern template class detail::camera_const_impl<camera_cref>;
	extern template class detail::camera_const_impl<camera_ref>;
	extern template class detail::camera_const_impl<camera>;
}
