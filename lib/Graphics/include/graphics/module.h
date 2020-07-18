#pragma once

#include "Ogre/Root.h"
#include "graphics/window.h"
#include "math/unit/time.h"

#include <span>
#include <chrono>
#include <memory>

namespace ot::graphics
{
	class module
	{
		struct impl;
		std::unique_ptr<impl> pimpl;

	public:
		module();
		module(module const&) = delete;
		module(module&&) noexcept = default;
		module& operator=(module const&) = delete;
		module& operator=(module&&) noexcept = default;
		~module();

		void initialize(char const* window_title, size_t number_threads);

		void on_window_events(std::span<window_event const> events);
		
		void update(math::seconds dt);
		bool render();

		// temporary
		void setup_scene();
	};
}