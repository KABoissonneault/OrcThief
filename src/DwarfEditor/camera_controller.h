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

		double yaw = 0.0;
		double pitch = 0.0;

		[[nodiscard]] egfx::object::camera_ref get_camera() noexcept;
		[[nodiscard]] egfx::window const& get_window() const noexcept;

		void translate(math::seconds dt);
		void rotate(math::seconds dt);
	public:
		bool handle_mouse_motion_event(SDL_MouseMotionEvent const& e);
		void update(math::seconds dt);
	};

	extern template camera_controller<application>;
}
