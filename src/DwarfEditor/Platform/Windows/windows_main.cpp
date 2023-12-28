#include "windows_main.h"

#include "main.h"
#include "window.h"

#include "Ogre/Root.h"

#include "SDL2/macro.h"
#include <SDL.h>
#undef main
#include <SDL_video.h>
#include <SDL_syswm.h>

HWND g_main_window;
char const* const k_window_title = "OrcThief";

namespace ot::dedit::windows
{
	HWND get_main_window()
	{
		return g_main_window;
	}
}

namespace
{
	HWND get_handle(SDL_Window& window)
	{
		SDL_SysWMinfo wm_info;
		SDL_VERSION(&wm_info.version);
		OT_SDL_ENSURE(SDL_GetWindowWMInfo(&window, &wm_info));
		return wm_info.info.win.window;
	}
}

extern "C" int SDL_main(int argc, char** argv)
{
	using namespace ot::dedit;

	if (!main::initialize(argc, argv))
		return -1;

	ot::sdl::unique_window window = ot::dedit::create_window(k_window_title);
	
	g_main_window = get_handle(*window);

	int const result = main::run(argc, argv, std::move(window));
	
	main::shutdown();

	return result;
}
