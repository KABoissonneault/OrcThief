#pragma once

#include "core/uptr.h"

#include "Ogre/MemoryAllocatorConfig.h"
#include "SDL2/window.h"

#include "graphics/module.h"
#include "graphics/mesh_definition.h"
#include "graphics/node/static_mesh.fwd.h"
#include "graphics/node/light.fwd.h"

#include <span>

namespace ot
{
	struct brush
	{
		graphics::mesh_definition mesh_def;
		ot::graphics::node::static_mesh node;
	};

	class application
	{
		sdl::unique_window main_window;
		graphics::module graphics;
		graphics::scene main_scene;

		std::vector<brush> brushes;
		ot::graphics::node::directional_light light;

		void update(math::seconds dt);

	public:
		bool initialize();
		void setup_default_scene();

		brush make_brush(std::span<math::plane const> planes, std::string const& name, math::vector3d position);

		void run();
	};
}