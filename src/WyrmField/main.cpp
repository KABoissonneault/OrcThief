#include "config.h"
#include "main_imgui.h"
#include "window.h"
#include "application/application.h"
#include "scene/scene.h"

#include "Ogre/ArchiveType.h"
#include "Ogre/ArchiveManager.h"
#include "Ogre/ConfigFile.h"
#include "Ogre/Root.h"
#include "Ogre/HlmsManager.h"
#include "Ogre/Components/Hlms/Unlit.h"
#include "Ogre/Components/Hlms/Pbs.h"

#include "math/unit/time.h"

#include "egfx/module.h"

#include <filesystem>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL_syswm.h>
#include "SDL2/macro.h"

#include <imgui_impl_sdl2.h>
#include <OgreAbiUtils.h>

namespace ot::wf
{
	void load_always_resources(Ogre::ConfigFile& program_config, std::filesystem::path const& resource_folder_path)
	{
		auto& resource_manager = Ogre::ResourceGroupManager::getSingleton();

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
				std::printf("warning: AlwaysLoad resource '%s' was requested but could not be found", full_path.string().c_str());
				continue;
			}

			resource_manager.addResourceLocation(full_path.string(), type_name);
		}

		Ogre::StringVectorPtr const names = resource_manager.listResourceNames(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		std::printf("Always Loaded resources:\n");
		for (Ogre::String const& resource : *names)
		{
			std::printf("- %s\n", resource.c_str());
		}
	}

	[[nodiscard]] bool load_program_config(ot::wf::config& config)
	{
		if (!std::filesystem::exists("config_wf.cfg"))
		{
			std::printf("Program needs 'config_wf.cfg' to run. Ensure a proper config file is in the current working directory\n");
			return false;
		}

		// Handle program config
		Ogre::ConfigFile program_config;
		program_config.load("config_wf.cfg");

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

	void load_hlms(std::filesystem::path const& resource_folder)
	{
		auto& root = Ogre::Root::getSingleton();
		Ogre::HlmsManager* const hlms_manager = root.getHlmsManager();

		Ogre::String main_folder;
		Ogre::StringVector library_folders;
		{
			Ogre::HlmsUnlit::getDefaultPaths(main_folder, library_folders);

			auto const main_path = resource_folder / main_folder;
			Ogre::Archive* const main_archive = Ogre::ArchiveManager::getSingleton().load(main_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);

			Ogre::ArchiveVec library_archives(library_folders.size());
			std::transform(library_folders.begin(), library_folders.end(), library_archives.begin(), [&resource_folder](auto const& library_folder)
			{
				auto const library_path = resource_folder / library_folder;
				return Ogre::ArchiveManager::getSingleton().load(library_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);
			});

			// Takes ownership of main_archive, but copies library_archives
			hlms_manager->registerHlms(OGRE_NEW Ogre::HlmsUnlit(main_archive, &library_archives));
		}

		{
			Ogre::HlmsPbs::getDefaultPaths(main_folder, library_folders);

			auto const main_path = resource_folder / main_folder;
			Ogre::Archive* const main_archive = Ogre::ArchiveManager::getSingleton().load(main_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);

			Ogre::ArchiveVec library_archives(library_folders.size());
			std::transform(library_folders.begin(), library_folders.end(), library_archives.begin(), [&resource_folder](auto const& library_folder)
			{
				auto const library_path = resource_folder / library_folder;
				return Ogre::ArchiveManager::getSingleton().load(library_path.string(), ot::ogre::archive_type::filesystem, true /*read-only*/);
			});

			// Takes ownership of main_archive, but copies library_archives
			auto const pbs_manager = OGRE_NEW Ogre::HlmsPbs(main_archive, &library_archives);
			hlms_manager->registerHlms(pbs_manager);
		}
	}
			
	void run_scene(SDL_Window& window, egfx::module& graphics, config const& program_config)
	{
		application& app = application::create_instance(window, graphics, program_config);

		try
		{
			app.load_game_data();
		}
		catch (std::exception& e)
		{
			std::fprintf(stderr, "Failed to load game data: %s", e.what());
		}

		try
		{
			app.run();
		}
		catch (...)
		{
			application::destroy_instance();
			throw;
		}

		application::destroy_instance();
	}

	int run_graphics(SDL_Window& main_window, config const& program_config)
	{
		// Graphics init
		egfx::module graphics;
		if (!initialize_graphics(graphics, main_window))
			return -1;

		std::string_view const resource_root = program_config.get_core().get_resource_root();
		std::filesystem::path const resource_folder_path(resource_root);
		load_hlms(resource_folder_path / "Ogre");
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

		run_scene(main_window, graphics, program_config);

		return 0;
	}

	int run_window(config const& program_config) noexcept
	{
		sdl::unique_window main_window = create_window("WyrmField 0.1");

		if (!imgui::initialize(*main_window))
		{
			std::fprintf(stderr, "Cannot initialize ImGui\n");
			return -1;
		}

		int result = -1;

		try
		{
			result = run_graphics(*main_window, program_config);
		}
		catch (std::exception const& e)
		{
			std::fprintf(stderr, "Application ended with exception: %s\n", e.what());
		}

		imgui::shutdown();

		return 0;
	}
}

int main(int argc, char** argv)
{
	Ogre::AbiCookie abi_cookie = Ogre::generateAbiCookie();
	Ogre::Root root{ &abi_cookie, "WyrmField/Ogre/plugins" OGRE_BUILD_SUFFIX ".cfg", "WyrmField/Ogre/ogre.cfg", "WyrmField/Ogre/ogre.log", "WyrmField" };
	
	ot::wf::config program_config;
	if (!ot::wf::load_program_config(/*out*/ program_config))
		return -1;

	if (!root.restoreConfig())
	{
		if (!root.showConfigDialog()) // If false, the user pressed Cancel
		{
			std::printf("User canceled configuration, shutting down game\n");
			return false;
		}
	}

	root.initialise(/*autoCreateWindow*/ false);

	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS);
	if (result != 0)
	{
		std::fprintf(stderr, "Cannot initialize SDL2 (%d): %s\n", result, SDL_GetError());
		return -1;
	}

	result = ot::wf::run_window(program_config);

	SDL_Quit();

	return result;
}
