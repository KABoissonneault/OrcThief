#include "Ogre/Root.h"
#include "Ogre/ConfigFile.h"
#include "Ogre/ResourceGroupManager.h"
#include "Ogre/Components/Hlms/Unlit.h"
#include "Ogre/Components/Hlms/Pbs.h"
#include "Ogre/ArchiveType.h"
#include "Ogre/ArchiveManager.h"
#include "Ogre/HlmsManager.h"

#include "graphics/window_type.h"
#include "graphics/module.h"

#include "math/unit/time.h"

#include "application.h"

#include <SDL.h>
#include "SDL2/window.h"
#include "SDL2/macro.h"

#include <filesystem>

namespace
{
	std::filesystem::path resource_folder_path;

	[[nodiscard]] bool load_program_config()
	{
		if (!std::filesystem::exists("config.cfg"))
		{
			std::printf("Program needs 'config.cfg' to run. Ensure a proper config file is in the current working directory\n");
			return false;
		}

		// Handle program config
		Ogre::ConfigFile program_config;
		program_config.load("config.cfg");

		Ogre::String const resource_folder = program_config.getSetting("ResourceRoot", "Core");
		if (resource_folder.empty())
		{
			std::printf("'ResourceRoot' under [Core] not found in 'config.cfg'. Ensure a proper config file is in the current working directory\n");
			return false;
		}

		auto load_it = program_config.getSettingsIterator("AlwaysLoad");
		resource_folder_path = std::filesystem::path(resource_folder);
		for (auto const& kv : load_it)
		{
			auto const type_name = kv.first;
			if (type_name != ot::ogre::archive_type::filesystem && type_name != ot::ogre::archive_type::zip && type_name != ot::ogre::archive_type::embedded_zip)
			{
				std::printf("Warning: AlwaysLoad resource of type '%s' is not supported\n", type_name.c_str());
				continue;
			}

			auto const resource_path = kv.second;
			auto const full_path = resource_folder_path / resource_path;

			if (!std::filesystem::exists(full_path))
			{
				std::printf("Warning: AlwaysLoad resource '%s' was requested but could not be found", full_path.string().c_str());
				continue;
			}
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(full_path.string(), type_name);
		}

		return true;
	}

	void load_hlms(std::filesystem::path const& resource_folder)
	{
		auto & root = Ogre::Root::getSingleton();
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
			hlms_manager->registerHlms(OGRE_NEW Ogre::HlmsPbs(main_archive, &library_archives));
		}
	}
}

extern "C" int main(int argc, char** argv)
{
	auto const plugin_path = "Ogre/plugins" OGRE_BUILD_SUFFIX ".cfg";
	auto const config_path = "Ogre/ogre.cfg";
	auto const log_path = "Ogre/ogre.log";

	Ogre::Root root(plugin_path, config_path, log_path);

	// Handle config.cfg
	if (!load_program_config())
	{
		return -1;
	}

	// Handle RenderSystem config
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

	ot::application app;
	if (!app.initialize())
	{
		return -1;
	}

	load_hlms(resource_folder_path);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

	app.setup_default_scene();
	
	app.run();

	SDL_Quit();

	return 0;
}
