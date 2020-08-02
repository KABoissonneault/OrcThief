#pragma once

#include "selection/context.h"
#include "map.h"

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "graphics/module.h"
#include "graphics/scene.h"
#include "graphics/mesh_definition.h"
#include "graphics/node/static_mesh.h"
#include "graphics/node/light.h"

#include <span>

namespace ot
{
	class application
	{
		sdl::unique_window main_window;
		graphics::module graphics;
		graphics::scene main_scene;

		ot::graphics::node::directional_light light;

		map current_map;

		uptr<selection::context> selection_context;

		void update(math::seconds dt);
	
	public:
		bool initialize();
		void setup_default_scene();

		brush make_brush(std::span<math::plane const> planes, std::string const& name, math::vector3d position);

		void run();
	};
}
