#include "datablock.h"
#include "window.h"
#include "config.h"
#include "console.h"
#include "imgui/module.h"
#include "application/application.h"

#include "Ogre/Root.h"
#include "Ogre/ConfigFile.h"
#include "Ogre/ResourceGroupManager.h"
#include "Ogre/ArchiveType.h"

#include "egfx/window_type.h"
#include "egfx/module.h"

#include "math/unit/time.h"

#include <SDL.h>
#undef main
#include <SDL_syswm.h>
#include "SDL2/window.h"
#include "SDL2/macro.h"

#include <filesystem>

namespace ot::dedit::main
{
	namespace
	{
		[[nodiscard]] void load_always_resources(Ogre::ConfigFile& program_config, std::filesystem::path const& resource_folder_path)
		{
			// Load the resources under [AlwaysLoad]
			auto load_it = program_config.getSettingsIterator("AlwaysLoad");
			for (auto const& kv : load_it)
			{
				auto const type_name = kv.first;
				if (type_name != ot::ogre::archive_type::filesystem && type_name != ot::ogre::archive_type::zip && type_name != ot::ogre::archive_type::embedded_zip)
				{
					std::printf("warning: AlwaysLoad resource of type '%s' is not supported\n", type_name.c_str());
					continue;
				}

				auto const resource_path = kv.second;
				auto const full_path = resource_folder_path / resource_path;

				if (!std::filesystem::exists(full_path))
				{
					std::printf("wrning: AlwaysLoad resource '%s' was requested but could not be found", full_path.string().c_str());
					continue;
				}
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(full_path.string(), type_name);
			}
		}

		[[nodiscard]] bool load_program_config(ot::dedit::config& config)
		{
			if (!std::filesystem::exists("config.cfg"))
			{
				std::printf("Program needs 'config.cfg' to run. Ensure a proper config file is in the current working directory\n");
				return false;
			}

			// Handle program config
			Ogre::ConfigFile program_config;
			program_config.load("config.cfg");

			if (!config.load(program_config))
				return false;

			std::string_view const resource_root = config.get_core().get_resource_root();
			std::filesystem::path const resource_folder_path(resource_root);

			load_always_resources(program_config, resource_folder_path);

			return true;
		}

		std::string get_native_handle_string(SDL_Window& window)
		{
			SDL_SysWMinfo wm_info;
			SDL_VERSION(&wm_info.version);
			OT_SDL_ENSURE(SDL_GetWindowWMInfo(&window, &wm_info));
			switch (wm_info.subsystem)
			{
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
			case SDL_SYSWM_WINDOWS: return std::to_string(reinterpret_cast<uintptr_t>(wm_info.info.win.window));
#endif
			default: OT_SDL_FAILURE("SDL_Window subsystem not implemented");
			}
		}

		bool is_fullscreen(SDL_Window& window)
		{
			return (SDL_GetWindowFlags(&window) & SDL_WINDOW_FULLSCREEN) != 0;
		}

		ot::egfx::window_parameters make_window_parameters(SDL_Window& physical_window)
		{
			ot::egfx::window_parameters params;
			params.window_handle = get_native_handle_string(physical_window);
			params.event_id = ot::egfx::window_id{ SDL_GetWindowID(&physical_window) };
			params.window_title = SDL_GetWindowTitle(&physical_window);
			params.fullscreen = is_fullscreen(physical_window);
			SDL_GetWindowSize(&physical_window, &params.width, &params.height);
			return params;
		}

		bool initialize_graphics(ot::egfx::module& g, SDL_Window& window)
		{
			auto const window_params = make_window_parameters(window);
			return g.initialize(window_params);
		}

		struct state
		{
			Ogre::Root root;
			config program_config;
		};

		state* g_state;
	}

	bool initialize()
	{
		g_state = new state{ {"Ogre/plugins" OGRE_BUILD_SUFFIX ".cfg", "Ogre/ogre.cfg", "Ogre/ogre.log"} };
		if (!load_program_config(g_state->program_config))
			return false;

		auto& root = g_state->root;

		if (!root.restoreConfig())
		{
			if (!root.showConfigDialog()) // If false, the user pressed Cancel
			{
				std::printf("User canceled configuration, shutting down game\n");
				return false;
			}
		}

		root.initialise(false /*create window*/);

		int const result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS);
		if (result != 0)
		{
			std::fprintf(stderr, "Cannot initialize SDL2 (%d): %s\n", result, SDL_GetError());
			return false;
		}

		return true;
	}

	int run(int argc, char** argv, sdl::unique_window main_window)
	{
		auto& program_config = g_state->program_config;
		std::string_view const resource_root = program_config.get_core().get_resource_root();
		std::filesystem::path const resource_folder_path(resource_root);

		if (!imgui::initialize(*main_window) || !imgui::build_fonts(resource_folder_path))
		{
			std::fprintf(stderr, "Cannot initialize ImGui\n");
			return -1;
		}

		{
			// Graphics init
			egfx::module graphics;
			if (!initialize_graphics(graphics, *main_window))
				return -1;		

			ot::dedit::datablock::load_hlms(resource_folder_path / "Ogre");
			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

			console::initialize();

			// Application
			application app(std::move(main_window), graphics, program_config);
			app.run();
		}

		// Exit
		imgui::shutdown();

		return 0;
	}

	void shutdown()
	{
		SDL_Quit();
		delete g_state;
	}
}