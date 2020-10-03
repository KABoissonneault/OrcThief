#pragma once

#include "egfx/window_type.h"
#include "egfx/scene.fwd.h"

#include "Ogre/Root.h"
#include "math/unit/time.h"

#include <span>
#include <string>

namespace ot::egfx
{
	struct window_parameters
	{
		std::string window_handle; // string representation of the native window handle
		window_id event_id; // id of the window, for use when window_event's are provided later
		std::string window_title;
		bool fullscreen;
		int width;
		int height;
	};

	class module
	{
		class impl;
		std::unique_ptr<impl> pimpl;

	public:
		module();
		module(module const&) = delete;
		module(module&&) noexcept = default;
		module& operator=(module const&) = delete;
		module& operator=(module&&) noexcept = default;
		~module();

		[[nodiscard]] bool initialize(window_parameters const& window_params);

		[[nodiscard]] scene create_scene(std::string const& workspace, size_t number_threads);
		// Sets the current main scene. If none is set, the first you create is considered the main one
		void set_main_scene(scene& s);

		void on_window_events(std::span<window_event const> events);
		
		// Clears the rendering state, preparing the system for this frame's rendering
		// Must be called before all rendering
		void pre_update();

		// Called at the end of the rendering process, to render all of the graphics elements on the viewport(s)
		// Returns false if rendering has failed and graphics cannot continue
		[[nodiscard]] bool render();

		[[nodiscard]] window const& get_window(window_id id) const noexcept;
	};
}
