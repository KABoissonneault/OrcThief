#pragma once

#include "graphics/module.h"

#include "graphics/window_type.h"
#include "window.h"

#include "math/unit/time.h"


namespace ot::graphics
{
	class module::impl
	{
		window main_window;

	public:
		void initialize(window_parameters const& window_params);

		[[nodiscard]] scene create_scene(size_t number_threads);

		void on_window_events(std::span<window_event const> events);

		void update(math::seconds dt);
		[[nodiscard]] bool render();

		[[nodiscard]] window const& get_window(window_id id) const noexcept;
	};
}
