#include "application.h"

#include "graphics/scene.h"
#include "graphics/camera.h"
#include "graphics/node/static_mesh.h"
#include "graphics/node/light.h"

#include "Ogre/ConfigOptionMap.h"
#include "Ogre/RenderSystem.h"

#include "SDL2/window.h"
#include "SDL2/macro.h"
#include <SDL_video.h>
#include <SDL_syswm.h>
#include <SDL_events.h>

namespace ot
{
	namespace
	{
		math::plane const cube_planes[6] = {
			{{0, 0, 1}, 0.5},
			{{1, 0, 0}, 0.5},
			{{0, 1, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{0, 0, -1}, 0.5},
		};

		auto const sqrt_half = 0.70710678118654752440084436210485;
		math::plane const octagon_planes[] = {
			{{0, 1, 0}, 0.5},
			{{0, -1, 0}, 0.5},
			{{1, 0, 0}, 0.5},
			{{-1, 0, 0}, 0.5},
			{{0, 0, 1}, 0.5},
			{{0, 0, -1}, 0.5},
			{{sqrt_half, 0, sqrt_half}, 0.5},
			{{sqrt_half, 0, -sqrt_half}, 0.5},
			{{-sqrt_half, 0, sqrt_half}, 0.5},
			{{-sqrt_half, 0, -sqrt_half}, 0.5},
		};

		math::plane const pyramid_planes[] = {
			{{0, -1, 0}, 0.5},
			{{sqrt_half, sqrt_half, 0}, 0.5},
			{{-sqrt_half, sqrt_half, 0}, 0.5},
			{{0, sqrt_half, sqrt_half}, 0.5},
			{{0, sqrt_half, -sqrt_half}, 0.5},
		};

		char const* const k_window_title = "OrcThief";

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

		ot::sdl::unique_window create_window()
		{
			auto const& config = Ogre::Root::getSingleton().getRenderSystem()->getConfigOptions();

			bool const fullscreen = get_fullscreen(config);
			auto const [width, height] = get_resolution(config);

			SDL_Window* physical_window;
			if (fullscreen)
			{
				physical_window = SDL_CreateWindow(k_window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
			} else
			{
				physical_window = SDL_CreateWindow(k_window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
			}

			return ot::sdl::unique_window{ physical_window };
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

		bool is_fullscren(SDL_Window& window)
		{
			return (SDL_GetWindowFlags(&window) & SDL_WINDOW_FULLSCREEN) != 0;
		}

		ot::graphics::window_parameters make_window_parameters(SDL_Window& physical_window)
		{
			ot::graphics::window_parameters params;
			params.window_handle = get_native_handle_string(physical_window);
			params.event_id = ot::graphics::window_id{ SDL_GetWindowID(&physical_window) };
			params.window_title = SDL_GetWindowTitle(&physical_window);
			params.fullscreen = is_fullscren(physical_window);
			SDL_GetWindowSize(&physical_window, &params.width, &params.height);
			return params;
		}

		size_t get_number_threads()
		{
			return Ogre::PlatformInformation::getNumLogicalCores();
		}

		void initialize_graphics(ot::graphics::module& g, SDL_Window& window)
		{
			auto const window_params = make_window_parameters(window);
			g.initialize(window_params);
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
	}

	bool application::initialize()
	{
		main_window = create_window();
		initialize_graphics(graphics, *main_window);

		main_scene = graphics.create_scene(get_number_threads() - 1);

		return true;
	}
	 
	void application::setup_default_scene()
	{
		brushes.emplace_back(make_brush(cube_planes, "Cube", { 0.0, 0.0, 0.0 }));
		brushes.emplace_back(make_brush(octagon_planes, "Octagon", { 2.5, 0.0, 0.0 }));
		brushes.emplace_back(make_brush(pyramid_planes, "Pyramid", { -2.5, 0.0, 0.0 }));

		auto& camera = get_camera(main_scene);
		set_position(camera, { 0.0, 2.5, -7.5 });
		look_at(camera, { 0.0, 0.0, 0.0 });

		light = graphics::node::create_directional_light(main_scene, get_root_node(main_scene));
		set_position(light, { 10.0, 10.0, -10.0 });
		set_direction(light, normalized(math::vector3d{ -1.0, -1.0, 1.0 }));

		select(brushes[0]);
	}

	brush application::make_brush(std::span<math::plane const> planes, std::string const& name, math::vector3d position)
	{
		auto mesh = graphics::mesh_definition::make_from_planes(planes);
		auto node = graphics::node::create_static_mesh(main_scene, get_root_node(main_scene), name, mesh);
		set_position(node, position);
		return { std::move(mesh), std::move(node) };
	}

	void application::run()
	{
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

			handle_window_events(graphics);

			{
				SDL_Event e;
				while (SDL_PollEvent(&e));
			}

			// Update
			while (frame_accumulator >= frame_time)
			{
				update(frame_time);
				frame_accumulator -= frame_time;
			}

			// Rendering
			if (!graphics.render())
				break;

			auto const current_frame = std::chrono::steady_clock::now();
			frame_accumulator += std::min(1._s, std::chrono::duration_cast<ot::math::seconds>(current_frame - last_frame));
			last_frame = current_frame;
		}
	}

	void application::update(math::seconds dt)
	{
		graphics.update(dt);

		for (auto& brush : brushes)
		{
			rotate_around(brush.node, math::vector3d{ 0.0, 1.0, 0.0 }, dt.count());
		}
	}
	
	void application::select(brush& brush)
	{
		selected_brush = graphics::node::create_static_wireframe_mesh(main_scene, brush.node, "SelectedBrush", brush.mesh_def);
	}
}