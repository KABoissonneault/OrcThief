#include "datablock.h"
#include "window.h"
#include "config.h"
#include "console.h"
#include "imgui/module.h"
#include "application/application.h"

#include "Ogre/Root.h"
#include "Ogre/ConfigFile.h"
#include "Ogre/ResourceGroupManager.h"

#include "egfx/window_type.h"
#include "egfx/module.h"

#include "math/unit/time.h"

#include <SDL.h>
#undef main
#include <SDL_syswm.h>
#include "SDL2/window.h"
#include "SDL2/macro.h"

#include <filesystem>
#include <OgreAbiUtils.h>

namespace ot::dedit::main
{
	namespace
	{
		[[nodiscard]] void load_always_resources(config const& program_config, std::filesystem::path const& resource_folder_path)
		{
			for (config::resource_load_item const& always_load_item : program_config.get_always_load_resources())
			{
				std::string const& type_name = always_load_item.type;
				std::filesystem::path const full_path = resource_folder_path / always_load_item.path;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(full_path.string(), type_name);
			}
		}

		[[nodiscard]] bool load_program_config(config& config, char const* game_config_path)
		{
			if (!std::filesystem::exists("config_de.cfg"))
			{
				std::printf("Editor needs 'config_de.cfg' to run. Ensure a proper config file is in the current working directory\n");
				return false;
			}

			// Handle program config
			Ogre::ConfigFile editor_config;
			editor_config.load("config_de.cfg");

			Ogre::ConfigFile game_config;
			if (game_config_path != nullptr)
			{
				if (!std::filesystem::exists(game_config_path))
				{
					std::printf("Specified game config '%s' does not exist\n", game_config_path);
					return false;
				}

				game_config.load(game_config_path);
			}

			if (!config.load(editor_config, game_config_path != nullptr ? &game_config : nullptr))
				return false;
			
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

		config program_config;
		Ogre::Root* ogre_root;
	}

	bool initialize(int argc, char** argv)
	{
		char const* game_config_path = nullptr;
		while (*argv)
		{
			if (std::strcmp(*argv, "game") == 0)
			{
				++argv;
				if (!*argv)
				{
					std::printf("Missing path after argument 'game'");
					return false;
				}

				game_config_path = *argv;
			}
			++argv;
		}

		if (!load_program_config(program_config, game_config_path))
			return false;

		Ogre::AbiCookie abi_cookie = Ogre::generateAbiCookie();
		ogre_root = new Ogre::Root( 
			&abi_cookie
			, "DwarfEditor/Ogre/plugins" OGRE_BUILD_SUFFIX ".cfg"
			, "DwarfEditor/Ogre/ogre.cfg"
			, "DwarfEditor/Ogre/ogre.log"
			, std::string(program_config.get_core().get_name())
		);
		
		std::string_view const resource_root = program_config.get_core().get_editor_resource_root();
		std::filesystem::path const resource_folder_path(resource_root);

		load_always_resources(program_config, resource_folder_path);

		auto& root = *ogre_root;

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
		std::string_view const editor_resource_root = program_config.get_core().get_editor_resource_root();
		std::filesystem::path const editor_resource_folder_path(editor_resource_root);

		if (!imgui::initialize(*main_window, program_config) || !imgui::build_fonts())
		{
			std::fprintf(stderr, "Cannot initialize ImGui\n");
			return -1;
		}

		{
			// Graphics init
			egfx::module graphics;
			if (!initialize_graphics(graphics, *main_window))
				return -1;		

			ot::dedit::datablock::load_hlms(editor_resource_folder_path / "Ogre");
			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

			ot::dedit::datablock::create_default_materials();

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
		delete ogre_root;
		ogre_root = nullptr;
	}
}