#include "graphics/window.h"

#include "Ogre/Root.h"
#include "Ogre/ConfigOptionMap.h"
#include "Ogre/RenderSystem.h"
#include "SDL2/macro.h"

#include <SDL_video.h>
#include <SDL_syswm.h>

namespace ot::graphics
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

		bool get_vsync(ot::ogre::config_option_map const& config)
		{
			bool vsync;
			return ot::ogre::render_system::get_vsync(config, vsync) ? vsync : false;
		}

		bool get_hardware_gamma_conversion(ot::ogre::config_option_map const& config)
		{
			bool gamma;
			return ot::ogre::render_system::get_hardware_gamma_conversion(config, gamma) ? gamma : false;
		}

		std::string get_native_handle_string(SDL_Window* window)
		{
			SDL_SysWMinfo wm_info;
			SDL_VERSION(&wm_info.version);
			OT_SDL_ENSURE(SDL_GetWindowWMInfo(window, &wm_info));
			switch (wm_info.subsystem)
			{
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
			case SDL_SYSWM_WINDOWS: return std::to_string(reinterpret_cast<uintptr_t>(wm_info.info.win.window));
#endif
			default: OT_SDL_FAILURE("SDL_Window subsystem not implemented");
			}
		}
	}

	window window::create(char const* title)
	{
		auto& root = Ogre::Root::getSingleton();
		auto const& config = root.getRenderSystem()->getConfigOptions();

		bool const fullscreen = get_fullscreen(config);
		auto const [x_resolution, y_resolution] = get_resolution(config);

		SDL_Window* physical_window;
		if (fullscreen)
		{
			physical_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x_resolution, y_resolution, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
		} else
		{
			physical_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x_resolution, y_resolution, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		}

		auto const misc_params = ot::ogre::render_window_misc_params{}
			.set_external_window_handle(get_native_handle_string(physical_window))
			.set_hardware_gamma_conversion(get_hardware_gamma_conversion(config))
			.set_vsync(get_vsync(config))
			.params;
		auto const render_window = root.createRenderWindow(title, x_resolution, y_resolution, fullscreen, &misc_params);

		return { physical_window, render_window };
	}

	window_id window::get_window_id() const noexcept
	{
		return window_id{ SDL_GetWindowID(physical_window.get()) };
	}
}
