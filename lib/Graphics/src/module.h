#pragma once

#include "graphics/module.h"

#include "graphics/window_type.h"

#include "math/unit/time.h"

#include "Ogre/Prerequisites.h"


namespace ot::graphics
{
	struct module::impl
	{		
		Ogre::Window* render_window;
		window_id render_window_id;

		void initialize(window_parameters const& window_params);

		scene create_scene(size_t number_threads);

		void on_window_events(std::span<window_event const> events);

		void update(math::seconds dt);
		bool render();
	};
}
