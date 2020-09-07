#pragma once

#include "selection/context.h"
#include "action/accumulator.h"
#include "map.h"
#include "hud/shadowed_text.h"

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "egfx/module.h"
#include "egfx/scene.h"
#include "egfx/node/light.h"
#include "egfx/node/manual.h"
#include "egfx/overlay/surface.h"

#include <span>
#include <optional>

namespace ot
{
	class application
	{
		class actions : public action::accumulator
		{
			std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> current_brush;
			std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> previous_brush;

		public:
			virtual void push_brush_action(uptr<action::brush_base, fwd_delete<action::brush_base>> action);
			void apply_actions(map& current_map);
			void undo_latest(map& current_map);
		};

		sdl::unique_window main_window;
		egfx::module& graphics;
		egfx::scene main_scene;

		egfx::node::directional_light light;

		map current_map;

		uptr<selection::context> selection_context;
		egfx::node::manual selection_render;

		egfx::overlay::surface debug_surface;
		std::optional<hud::shadowed_text> debug_text;
		
		actions selection_actions;

		bool wants_quit = false;

		void handle_events();
		void update(math::seconds dt);
		[[nodiscard]] bool render();

	public:
		application(sdl::unique_window window, egfx::module& graphics);

		[[nodiscard]] bool initialize();
		void setup_default_scene();

		[[nodiscard]] brush make_brush(std::span<math::plane const> planes, std::string const& name, math::point3d position);

		void run();
	};
}
