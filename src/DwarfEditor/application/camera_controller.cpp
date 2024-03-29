#include "camera_controller.h"

#include "input.h"
#include "application.h"

#include "egfx/object/camera.h"

namespace ot::dedit
{
	[[nodiscard]] egfx::camera_ref get_camera_impl(application& app)
	{
		return app.get_scene().get_camera();
	}

	[[nodiscard]] egfx::window const& get_window_impl(application const& app)
	{
		return app.get_render_window();
	}

	template<typename Application>
	egfx::camera_ref camera_controller<Application>::get_camera() noexcept
	{
		return get_camera_impl(static_cast<derived&>(*this));
	}

	template<typename Application>
	egfx::window const& camera_controller<Application>::get_window() const noexcept
	{
		return get_window_impl(static_cast<derived const&>(*this));
	}

	template<typename Application>
	void camera_controller<Application>::start_control()
	{
		controlling_camera = true;
		input::mouse::set_relative_mode(true);
	}

	template<typename Application>
	void camera_controller<Application>::clear_control()
	{
		yaw = 0;
		pitch = 0;
		controlling_camera = false;
		input::mouse::set_relative_mode(false);
	}

	template<typename Application>
	bool camera_controller<Application>::handle_keyboard_event(SDL_KeyboardEvent const& key)
	{
		// Just swallow keyboard input events while controlling the camera
		if (controlling_camera)
			return true;

		if (input::mouse::get_buttons() == input::mouse::button_type::right)
		{
			switch (key.keysym.scancode)
			{
			case SDL_SCANCODE_Q:
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_E:
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_D:
				if (key.state == SDL_PRESSED
					&& key.repeat == 0
					&& (key.keysym.mod & (KMOD_CTRL | KMOD_SHIFT | KMOD_ALT)) == 0)
				{
					start_control();
					return true;
				}
			}
		}

		return false;
	}

	template<typename Application>
	bool camera_controller<Application>::handle_text_event(SDL_TextInputEvent const& e)
	{
		// Just swallow keyboard input events while controlling the camera
		if (controlling_camera)
			return true;

		return false;
	}

	template<typename Application>
	bool camera_controller<Application>::handle_mouse_button_event(SDL_MouseButtonEvent const& e)
	{
		auto const button = input::mouse::button_type_from_sdl(e.button);
		if (controlling_camera && button == input::mouse::button_type::right && e.state != SDL_PRESSED)
		{
			return true;
		}

		return false;
	}

	template<typename Application>
	bool camera_controller<Application>::handle_mouse_motion_event(SDL_MouseMotionEvent const& e)
	{
		if (!controlling_camera)
		{
			if (input::mouse::get_buttons() == input::mouse::button_type::right)
				start_control();
			else
				return false;
		}
		// Only move the camera while the right-click is pressed
		else if (input::mouse::get_buttons() != input::mouse::button_type::right)
		{
			clear_control();
			return false;
		}

		egfx::camera_cref const camera = get_camera();
		egfx::window const& window = get_window();

		yaw += -static_cast<float>(e.xrel) / static_cast<float>(get_width(window)) * camera.get_rad_fov_x();
		pitch += -static_cast<float>(e.yrel) / static_cast<float>(get_height(window)) * camera.get_rad_fov_y();

		return true; 
	}

	template<typename Application>
	bool camera_controller<Application>::handle_mouse_wheel_event(SDL_MouseWheelEvent const& e)
	{
		if (!controlling_camera)
		{
			if (input::mouse::get_buttons() == input::mouse::button_type::right)
			{
				start_control();
			}
			else if(input::mouse::get_buttons() == input::mouse::button_type::none)
			{
				math::vector3f const velocity = { 0, 0, -e.y * camera_velocity * 0.25f };
				get_camera().move(velocity);
				return true;
			}
		}

		int y = e.y;
		while (y > 0)
		{
			camera_velocity *= 1.1f;
			--y;
		}
		while (y < 0 && camera_velocity > 1.0f)
		{
			camera_velocity *= 0.909090909f;
			++y;
		}

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
		if (input::keyboard::is_pressed(SDL_SCANCODE_E))
			velocity += { 0, 1, 0 };
		if (input::keyboard::is_pressed(SDL_SCANCODE_Q))
			velocity += { 0, -1, 0 };

		if (float_eq(velocity, { 0, 0, 0 }))
			return;

		velocity = normalized(velocity);
		velocity *= camera_velocity;

		get_camera().move(velocity * dt.count());
	}

	template<typename Application>
	void camera_controller<Application>::rotate(math::seconds dt)
	{
		if (float_eq(pitch, 0.0f) && float_eq(yaw, 0.0f))
			return;

		egfx::camera_ref const c = get_camera();
		c.local_pitch(pitch);
		c.world_yaw(yaw);

		pitch = 0.0;
		yaw = 0.0;
	}

	template<typename Application>
	void camera_controller<Application>::update(math::seconds dt)
	{
		if (!controlling_camera)
			return;

		// Only move the camera while the right-click is pressed
		if (input::mouse::get_buttons() != input::mouse::button_type::right)
		{
			clear_control();
			return;
		}

		rotate(dt);
		translate(dt);
	}

	template class camera_controller<application>;
}
