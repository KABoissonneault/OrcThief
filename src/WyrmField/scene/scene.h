#pragma once

#include "SDL2/window.h"

#include "math/unit/time.h"

#include "egfx/scene.h"
#include "egfx/node/light.h"
#include "egfx/object/camera.fwd.h"

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

			egfx::node::directional_light main_light;
			math::transform_matrix cube_transform;
			float y_buffer = 0.0f;

		public:
			scene(egfx::module& gfx_module, config const& program_config);

			void update(math::seconds dt);
			void render();

			egfx::object::camera_cref get_camera() const noexcept;
		};
	}
}
