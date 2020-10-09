#include "window.h"

#include "Ogre/Root.h"
#include "Ogre/RenderSystem.h"
#include <SDL_video.h>

namespace ot::dedit
{
	namespace
	{
		bool get_fullscreen(ot::ogre::config_option_map const& config)
		{
			bool fullscreen;
			return ot::ogre::render_system::get_fullscreen(config, fullscreen) ? fullscreen : false;
		}

		std::pair<int, int> get_resolution(ot::ogre::config_option_map const& config)
		{
			int width, height;
			return ot::ogre::render_system::get_resolution(config, width, height) ? std::make_pair(width, height) : std::make_pair(1280, 768);
		}
	}

	ot::sdl::unique_window create_window(char const* window_title)
	{
		auto const& config = Ogre::Root::getSingleton().getRenderSystem()->getConfigOptions();

		bool const fullscreen = get_fullscreen(config);
		auto const [width, height] = get_resolution(config);

		SDL_Window* physical_window;
		if (fullscreen)
		{
			physical_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
		} 
		else
		{
			physical_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
		}

		return ot::sdl::unique_window{ physical_window };
	}
}
