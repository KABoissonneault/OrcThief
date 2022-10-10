#include "config.h"
#include "window.h"
#include "main_imgui.h"
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
#include "egfx/window_type.h"
#include "egfx/object/camera.h"
#include "egfx/node/light.h"

#include <filesystem>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL_syswm.h>
#include "SDL2/macro.h"

#include <imgui_impl_sdl.h>
#include <im3d.h>

namespace ot::wf
{
	void load_always_resources(Ogre::ConfigFile& program_config, std::filesystem::path const& resource_folder_path)
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
				std::printf("warning: AlwaysLoad resource '%s' was requested but could not be found", full_path.string().c_str());
				continue;
			}
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(full_path.string(), type_name);
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

	void push_window_event(SDL_Event const& e, std::vector<egfx::window_event>& window_events)
	{
		using egfx::window_event;
		using egfx::window_id;
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

	Im3d::Vec3 get_cursor_ray(egfx::object::camera_cref const camera, math::transform_matrix const& camera_projection)
	{
		Im3d::AppData& ad = Im3d::GetAppData();

		int cursor_x, cursor_y;
		SDL_GetMouseState(&cursor_x, &cursor_y);

		float const norm_x = (static_cast<float>(cursor_x) / ad.m_viewportSize.x) * 2.0f - 1.0f;
		float const norm_y = -((static_cast<float>(cursor_y) / ad.m_viewportSize.y) * 2.0f - 1.0f);

		math::transform_matrix const camera_transform = camera.get_transformation();

		math::vector3f const ray_direction
		{
			norm_x / camera_projection[0][0]
			, norm_y / camera_projection[1][1]
			, -1.0f
		};

		return normalized(transform(ray_direction, camera_transform));
	}
			
	void run_scene(SDL_Window& window, egfx::module& graphics, config const& program_config)
	{
		scene main_scene(graphics, program_config);
		
		using namespace ot::math::literals;
		constexpr math::seconds fixed_step(0.02f);
		std::chrono::time_point current_frame = std::chrono::steady_clock::now();
		math::seconds time_buffer = fixed_step; // Start with one step

		bool wants_quit = false;
		while (!wants_quit)
		{
			// Events
			{
				std::vector<egfx::window_event> window_events;

				SDL_Event e;
				while (SDL_PollEvent(&e))
				{
					switch (e.type)
					{
					case SDL_WINDOWEVENT:
						ImGui_ImplSDL2_ProcessEvent(&e);
						push_window_event(e, window_events);

						if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(&window))
						{
							wants_quit = true;
						}

						break;

					case SDL_QUIT:
						wants_quit = true;
						break;
					}
				}

				graphics.on_window_events(window_events);
			}

			// Pre-update
			imgui::pre_update();
			graphics.pre_update();
			{
				Im3d::AppData& ad = Im3d::GetAppData();
				egfx::object::camera_cref const camera = main_scene.get_camera();
				math::transform_matrix const camera_projection = camera.get_projection();

				ad.m_cursorRayOrigin = ad.m_viewOrigin;
				ad.m_cursorRayDirection = get_cursor_ray(camera, camera_projection);

				Uint32 const mouse_state = SDL_GetMouseState(nullptr, nullptr);
				ad.m_keyDown[Im3d::Mouse_Left] = (mouse_state & SDL_BUTTON_LMASK) == SDL_BUTTON_LEFT;
			}

			// Fixed Update
			while (time_buffer >= fixed_step)
			{
				main_scene.update(fixed_step);

				time_buffer -= fixed_step;
			}

			// Render
			main_scene.render();

			if (!graphics.render())
				wants_quit = true;

			// End frame
			imgui::end_frame();

			std::chrono::time_point const last_frame = std::exchange(current_frame, std::chrono::steady_clock::now());
			time_buffer += current_frame - last_frame;
		}
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
			std::fprintf(stderr, "Application ended with exception: %s", e.what());
		}

		imgui::shutdown();

		return 0;
	}
}

int main(int argc, char** argv)
{
	Ogre::Root root{ "WyrmField/Ogre/plugins" OGRE_BUILD_SUFFIX ".cfg", "WyrmField/Ogre/ogre.cfg", "WyrmField/Ogre/ogre.log", "WyrmField" };
	
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
