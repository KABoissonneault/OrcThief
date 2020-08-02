#pragma once

#include "graphics/window_type.h"
#include "graphics/scene.fwd.h"

#include "Ogre/Root.h"
#include "math/unit/time.h"

#include <span>
#include <string>

namespace ot::graphics
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

		void initialize(window_parameters const& window_params);

		[[nodiscard]] scene create_scene(size_t number_threads);
		
		void on_window_events(std::span<window_event const> events);
		
		void update(math::seconds dt);
		// Returns false if rendering has failed and graphics cannot continue
		[[nodiscard]] bool render();

		[[nodiscard]] window const& get_window(window_id id) const noexcept;
	};
}
