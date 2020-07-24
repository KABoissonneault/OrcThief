#include "Ogre/Root.h"
#include "Ogre/ConfigFile.h"
#include "Ogre/ResourceGroupManager.h"
#include "Ogre/Components/Hlms/Unlit.h"
#include "Ogre/Components/Hlms/Pbs.h"
#include "Ogre/ArchiveType.h"
#include "Ogre/ArchiveManager.h"
#include "Ogre/HlmsManager.h"

#include "graphics/window.h"
#include "graphics/window_type.h"
#include "graphics/module.h"

#include "math/unit/time.h"

#include <SDL.h>

#include <filesystem>

namespace
{
	size_t get_number_threads()
	{
		return Ogre::PlatformInformation::getNumLogicalCores();
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

		static_cast<Ogre::HlmsPbs*>(hlms_manager->getHlms(Ogre::HLMS_PBS))->loadLtcMatrix();
	}

	void push_window_event(SDL_Event const& e, std::vector<ot::graphics::window_event>& window_events)
	{
		using ot::graphics::window_event;
		using ot::graphics::window_id;
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
		}
	}

	void handle_window_events(ot::graphics::module& g)
	{
		SDL_Event events[4]; // I'm not sure if I should expect more than 4 window events per frame
		while (int const count = SDL_PeepEvents(events, 4, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT))
		{
			using ot::graphics::window_event;
			using ot::graphics::window_id;
			std::vector<window_event> window_events;
			window_events.reserve(count);
			for (size_t i = 0; i < count; ++i)
			{
				push_window_event(events[i], window_events);
			}

			g.on_window_events(window_events);
		}
	}

	char const* const k_window_title = "OrcThief";
}

extern "C" int main(int argc, char** argv)
{
	Ogre::Root root;

	if (!std::filesystem::exists("config.cfg"))
	{
		std::printf("Program needs 'config.cfg' to run. Ensure a proper config file is in the current working directory\n");
		return -1;
	}

	// Handle program config
	Ogre::ConfigFile program_config;
	program_config.load("config.cfg");

	Ogre::String const resource_folder = program_config.getSetting("ResourceRoot", "Core");
	if (resource_folder.empty())
	{
		std::printf("'ResourceRoot' under [Core] not found in 'config.cfg'. Ensure a proper config file is in the current working directory\n");
		return -1;
	}

	auto load_it = program_config.getSettingsIterator("AlwaysLoad");
	auto const resource_folder_path = std::filesystem::path(resource_folder);
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

	ot::graphics::module g;
	g.initialize(k_window_title, get_number_threads() - 1);

	load_hlms(resource_folder_path);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);

	g.setup_scene();

	using namespace ot::math::literals;
	auto last_frame = std::chrono::steady_clock::now();
	auto const frame_time = 0.02_s;
	auto frame_accumulator = frame_time;

	bool quit = false;
	while (!quit) {
		// Events
		SDL_PumpEvents();
		
		if (SDL_HasEvent(SDL_QUIT))
		{
			quit = true;
			break;
		}

		handle_window_events(g);

		{
			SDL_Event e;
			while (SDL_PollEvent(&e)); // clear the other events
		}

		// Update
		while (frame_accumulator >= frame_time)
		{
			g.update(frame_time);
			frame_accumulator -= frame_time;
		}

		// Rendering
		if (!g.render())
			break;		

		auto const current_frame = std::chrono::steady_clock::now();
		frame_accumulator += std::min(1._s, std::chrono::duration_cast<ot::math::seconds>(current_frame - last_frame));
		last_frame = current_frame;
	}

	SDL_Quit();

	return 0;
}
