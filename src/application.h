#pragma once

#include "selection/context.h"
#include "map.h"
#include "hud/shadowed_text.h"

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "graphics/module.h"
#include "graphics/scene.h"
#include "graphics/mesh_definition.h"
#include "graphics/node/static_mesh.h"
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

		graphics::overlay::surface debug_surface;
		std::optional<hud::shadowed_text> debug_text;

		void update(math::seconds dt);
	
	public:
		bool initialize();
		void setup_default_scene();

		brush make_brush(std::span<math::plane const> planes, std::string const& name, math::point3d position);

		void run();
	};
}
