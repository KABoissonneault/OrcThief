#include "camera_controller.h"

#include "input.h"
#include "application.h"

#include "egfx/object/camera.h"

namespace ot::dedit
{
	[[nodiscard]] egfx::object::camera_ref get_camera_impl(application& app)
	{
		return app.get_scene().get_camera();
	}

	[[nodiscard]] egfx::window const& get_window_impl(application const& app)
	{
		return app.get_render_window();
	}

	template<typename Application>
	egfx::object::camera_ref camera_controller<Application>::get_camera() noexcept
	{
		return get_camera_impl(static_cast<derived&>(*this));
	}

	template<typename Application>
	egfx::window const& camera_controller<Application>::get_window() const noexcept
	{
		return get_window_impl(static_cast<derived const&>(*this));
	}

	template<typename Application>
	bool camera_controller<Application>::handle_mouse_motion_event(SDL_MouseMotionEvent const& e)
	{
		// Only move the camera while the right-click is pressed
		if (input::mouse::get_buttons() != input::mouse::button_type::right)
			return false;

		egfx::object::camera_ref const camera = get_camera();
		egfx::window const& window = get_window();

		yaw += -static_cast<float>(e.xrel) / static_cast<float>(get_width(window)) * camera.get_rad_fov_x();
		pitch += -static_cast<float>(e.yrel) / static_cast<float>(get_height(window)) * camera.get_rad_fov_y();

		return true; 
	}

	template<typename Application>
	void camera_controller<Application>::translate(math::seconds dt)
	{
		math::vector3f velocity = { 0, 0, 0 };

		if (input::keyboard::is_pressed(SDL_SCANCODE_W))
			velocity += { 0, 0, -1 };
		if (input::keyboard::is_pressed(SDL_SCANCODE_A))
			velocity += { -1, 0, 0 };
		if (input::keyboard::is_pressed(SDL_SCANCODE_S))
			velocity += { 0, 0, 1 };
		if (input::keyboard::is_pressed(SDL_SCANCODE_D))
			velocity += { 1, 0, 0 };
		if (input::keyboard::is_pressed(SDL_SCANCODE_SPACE))
			velocity += { 0, 1, 0 };
		if (input::keyboard::is_pressed(SDL_SCANCODE_C))
			velocity += { 0, -1, 0 };

		if (float_eq(velocity, { 0, 0, 0 }))
			return;

		velocity = normalized(velocity);
		velocity *= 3.0f; // TODO: parameterize

		get_camera().move(velocity * dt.count());
	}

	template<typename Application>
	void camera_controller<Application>::rotate(math::seconds dt)
	{
		if (float_eq(pitch, 0.0f) && float_eq(yaw, 0.0f))
			return;

		egfx::object::camera_ref const c = get_camera();
		c.local_pitch(pitch);
		c.world_yaw(yaw);

		pitch = 0.0;
		yaw = 0.0;
	}

	template<typename Application>
	void camera_controller<Application>::update(math::seconds dt)
	{
		// Only move the camera while the right-click is pressed
		if (input::mouse::get_buttons() != input::mouse::button_type::right)
			return;

		rotate(dt);
		translate(dt);
	}

	template class camera_controller<application>;
}
