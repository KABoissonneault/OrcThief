#pragma once

#include "Ogre/Root.h"
#include "graphics/window.h"

#include <span>

namespace ot::graphics
{
	class module
	{
		Ogre::SceneManager* scene_manager;
		Ogre::Camera* main_camera;
		Ogre::CompositorWorkspace* main_workspace;
		ot::graphics::window main_window;
	public:
		void initialize(char const* window_title, int number_threads);

		void on_window_events(std::span<window_event const> events);

		bool render();
	};
}