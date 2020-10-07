#pragma once

#include "application/camera_controller.h"
#include "application/mouse_controller.h"
#include "application/action_handler.h"
#include "application/menu.h"

#include "config.h"
#include "map.h"
#include "selection/context.h"

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "egfx/module.h"
#include "egfx/scene.h"
#include "egfx/node/light.h"

#include <span>
#include <optional>

namespace ot::dedit
{
	class application 
		: private camera_controller<application>
		, private menu<application>
	{
		friend class camera_controller<application>;
		friend class menu<application>;

		sdl::unique_window main_window;
		egfx::module& graphics;
		egfx::scene main_scene;

		egfx::node::directional_light light;

		map current_map;
		std::string map_path;

		uptr<selection::context> selection_context;
		
		action_handler selection_actions;
		mouse_controller mouse;

		bool wants_quit = false;

		void start_frame();
		void handle_events();
		void pre_update();
		void update(math::seconds dt);
		[[nodiscard]] bool render();
		void end_frame();

		// Accessors for our friend classes
		camera_controller& get_camera() noexcept { return *this; }
		action_handler& get_actions() noexcept { return selection_actions; }
		menu& get_menu() noexcept { return *this; }

		map& get_current_map() noexcept { return current_map; }

		void update_im3d();

	public:
		application(sdl::unique_window window, egfx::module& graphics, config const& program_config);

		[[nodiscard]] brush make_brush(std::span<math::plane const> planes, std::string const& name, math::point3f position);

		void run();
		// Called while "run" is executing. Tells the application to end the execution of "run"
		void quit() { wants_quit = true; }

		[[nodiscard]] egfx::scene& get_scene() noexcept { return main_scene; }
		[[nodiscard]] egfx::window const& get_render_window() const noexcept { return graphics.get_window(egfx::window_id{ SDL_GetWindowID(main_window.get()) }); }
	};
}
