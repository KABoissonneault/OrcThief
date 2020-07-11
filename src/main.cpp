#include "Ogre/Root.h"
#include "Ogre/Window.h"
#include "Ogre/WindowEventUtilities.h"
#include "Ogre/Compositor/CompositorManager2.h"
#include "Ogre/Compositor/CompositorWorkspace.h"
#include "Ogre/RenderSystem.h"

#include "graphics/window.h"
#include "graphics/window_type.h"
#include "graphics/module.h"

#include <SDL.h>

#include <memory>

int get_number_threads()
{
	return 4; // TODO
}

char const* const k_window_title = "OrcThief";

extern "C" int main(int argc, char** argv)
{
	Ogre::Root root;
	if (!root.restoreConfig())
	{
		if (!root.showConfigDialog()) // If false, the user pressed Cancel
		{
			std::printf("User canceled configuration, shutting down game\n");
			return 0;
		}
	}

	{
		int const result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS);
		if (result != 0)
		{
			std::fprintf(stderr, "Cannot initialize SDL2 (%d): %s\n", result, SDL_GetError());
			return -1;
		}
	}

	root.initialise(false /*create window*/);

	ot::graphics::module g;
	g.initialize(k_window_title, get_number_threads() - 1);

	bool quit = false;
	while (!quit) {
		// Events
		SDL_PumpEvents();

		using ot::graphics::window_event;
		using ot::graphics::window_id;
		SDL_Event e;
		std::vector<window_event> window_events;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_MOVED:
					window_events.push_back({ window_id(e.window.windowID), window_event::moved{e.window.data1, e.window.data2} });
					break;
				case SDL_WINDOWEVENT_RESIZED:
					window_events.push_back({ window_id(e.window.windowID), window_event::resized{e.window.data1, e.window.data2} });
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					window_events.push_back({ window_id(e.window.windowID), window_event::focus_gained{} });
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					window_events.push_back({ window_id(e.window.windowID), window_event::focus_lost{} });
					break;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}
		}

		if (quit)
			break;

		g.on_window_events(window_events);

		// Rendering
		if (!g.render())
			break;		
	}

	SDL_Quit();

	return 0;
}
