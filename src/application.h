#pragma once

#include "selection/context.h"
#include "action/accumulator.h"
#include "map.h"
#include "hud/shadowed_text.h"

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "graphics/module.h"
#include "graphics/scene.h"
#include "graphics/node/light.h"
#include "graphics/overlay/surface.h"

#include <span>
#include <optional>

namespace ot
{
	class application
	{
		sdl::unique_window main_window;
		graphics::module graphics;
		graphics::scene main_scene;

		graphics::node::directional_light light;

		map current_map;

		uptr<selection::context> selection_context;
		graphics::node::manual selection_render;

		graphics::overlay::surface debug_surface;
		std::optional<hud::shadowed_text> debug_text;

		class actions : public action::accumulator
		{
			std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> current_brush;
			std::vector<uptr<action::brush_base, fwd_delete<action::brush_base>>> previous_brush;

		public:
			virtual void push_brush_action(uptr<action::brush_base, fwd_delete<action::brush_base>> action);
			void apply_actions(map& current_map);
			void undo_latest(map& current_map);
		};
		
		actions selection_actions;

		void handle_events();
		void update(math::seconds dt);
		[[nodiscard]] bool render();

	public:
		[[nodiscard]] bool initialize();
		void setup_default_scene();

		[[nodiscard]] brush make_brush(std::span<math::plane const> planes, std::string const& name, math::point3d position);

		void run();
	};
}
