#pragma once

#include "math/unit/time.h"

#include "m3/character.h"

#include "scene/scene.h"

#include <vector>
#include <span>

struct SDL_Window;
union SDL_Event;

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
			std::vector<m3::player_character_data> player_data;

			bool wants_quit = false;
			bool draw_debug = false;

			int hud_state = 0;
			int hud_param = 0;

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

			std::span<m3::player_character_data> get_player_data() noexcept;

		private:
			void process_events();

			bool handle_hud_input(SDL_Event const& e);

			void draw_hud();
			void draw_player_vitals();
			void draw_player_sheet();
		};
	}
}
