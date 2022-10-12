#pragma once

#include "SDL2/window.h"

#include "math/unit/time.h"

#include "egfx/scene.h"
#include "egfx/object/camera.fwd.h"

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

namespace ot
{
	namespace egfx
	{
		class module;
	}

	namespace wf
	{
		class config;

		class scene
		{
			egfx::module* gfx;
			egfx::scene gfx_scene;

			entt::registry scene_registry;
			std::vector<entt::entity> scene_entities;
		public:
			scene(egfx::module& gfx_module, config const& program_config);

			void update(math::seconds dt);
			void render();

			egfx::object::camera_cref get_camera() const noexcept;
		};
	}
}
