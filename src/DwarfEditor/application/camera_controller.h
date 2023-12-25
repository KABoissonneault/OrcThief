#pragma once

#include "math/unit/time.h"
#include "math/quaternion.h"
#include "egfx/object/camera.fwd.h"
#include "egfx/window.h"

#include <SDL_events.h>

namespace ot::dedit
{
	class application;

	template<typename Application>
	class camera_controller
	{
		using derived = Application;

		float yaw = 0.0f;
		float pitch = 0.0f;
		float camera_velocity = 3.0f;
		bool controlling_camera = false;

		void start_control();
		void clear_control();

		[[nodiscard]] egfx::object::camera_ref get_camera() noexcept;
		[[nodiscard]] egfx::window const& get_window() const noexcept;

		void translate(math::seconds dt);
		void rotate(math::seconds dt);
	public:
		bool handle_keyboard_event(SDL_KeyboardEvent const& key);
		bool handle_mouse_button_event(SDL_MouseButtonEvent const& e);
		bool handle_mouse_motion_event(SDL_MouseMotionEvent const& e);
		bool handle_mouse_wheel_event(SDL_MouseWheelEvent const& e);
		void update(math::seconds dt);
	};

	extern template class camera_controller<application>;
}
