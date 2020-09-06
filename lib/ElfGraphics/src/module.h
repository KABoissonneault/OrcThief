#pragma once

#include "egfx/module.h"

#include "egfx/window_type.h"
#include "window.h"
#include "imgui/system.h"

#include "core/uptr.h"

#include "math/unit/time.h"

#include "Ogre/Components/Overlay/System.h"

namespace ot::egfx
{
	class module::impl
	{
		window main_window;
		uptr<Ogre::v1::OverlaySystem> overlay_system;
		uptr<imgui::system> imgui;

	public:
		~impl();

		[[nodiscard]] bool initialize(window_parameters const& window_params);

		[[nodiscard]] scene create_scene(size_t number_threads);

		void on_window_events(std::span<window_event const> events);

		void update(math::seconds dt);

		void start_frame();
		[[nodiscard]] bool render();

		[[nodiscard]] window const& get_window(window_id id) const noexcept;
	};
}
