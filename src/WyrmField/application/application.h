#pragma once

#include "math/unit/time.h"

#include "m3/character.h"

#include "scene/scene.h"

#include <vector>
#include <span>

struct SDL_Window;

namespace ot
{
	namespace egfx
	{
		class module;
	}

	namespace wf
	{
		class config;

		class application
		{
			SDL_Window* window;
			egfx::module* gfx_module;
			config const* program_config;
			scene main_scene;

			std::vector<m3::enemy_template> enemy_templates;

			application(SDL_Window& window, egfx::module& gfx_module, config const& program_config);

		public:
			static constexpr math::seconds fixed_step{ 0.02f };

			static application& create_instance(SDL_Window& window, egfx::module& gfx_module, config const& program_config);
			static void destroy_instance() noexcept;
			[[nodiscard]] static application& get_instance();

			void run();

			void save_game_data();
			void load_game_data();

			std::span<m3::enemy_template> get_enemy_templates() noexcept;
			void add_enemy_template(m3::enemy_template const& t);
		};
	}
}
