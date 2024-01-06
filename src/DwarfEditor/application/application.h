#pragma once

#include "application/camera_controller.h"
#include "application/mouse_controller.h"
#include "application/action_handler.h"
#include "application/menu.h"
#include "application/map_handler.h"
#include "application/basic_mesh_repo.h"

#include "config.h"
#include "map.h"
#include "selection/context.h"

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "egfx/module.h"
#include "egfx/scene.h"

#include <span>
#include <optional>

namespace ot::dedit
{
	class application 
		: private camera_controller<application>
		, private menu<application>
		, private map_handler<application>
	{
		friend class camera_controller<application>;
		friend class menu<application>;
		friend class map_handler<application>;

		sdl::unique_window main_window;
		egfx::module& graphics;
		egfx::scene main_scene;

		map current_map;

		uptr<selection::context> selection_context;
		
		action_handler selection_actions;
		mouse_controller mouse;

		basic_mesh_repo mesh_repo;

		bool wants_quit = false;

		void start_frame();
		void handle_events();
		void pre_update();
		void update(math::seconds dt);
		[[nodiscard]] bool render();
		void end_frame();

		// Accessors for our friend classes
		camera_controller& get_camera() noexcept { return *this; }
		action_handler& get_action_handler() noexcept { return selection_actions; }
		action_handler const& get_action_handler() const noexcept { return selection_actions; }
		menu& get_menu() noexcept { return *this; }
		map_handler& get_map_handler() noexcept { return *this; }

		map& get_current_map() noexcept { return current_map; }

		void update_im3d();

	public:
		application(sdl::unique_window window, egfx::module& graphics, config const& program_config);

		void run();
		// Called while "run" is executing. Tells the application to end the execution of "run"
		void quit();
		[[nodiscard]] bool is_quitting() const noexcept { return wants_quit; }
		// Called while the application wants to quit. 
		void cancel_quit() { wants_quit = false; }

		[[nodiscard]] egfx::scene& get_scene() noexcept { return main_scene; }
		[[nodiscard]] egfx::window const& get_render_window() const noexcept { return graphics.get_window(egfx::window_id{ SDL_GetWindowID(main_window.get()) }); }

		[[nodiscard]] basic_mesh_repo const& get_mesh_repo() const noexcept { return mesh_repo; }
	};
}
