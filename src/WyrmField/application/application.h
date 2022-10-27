#pragma once

#include "core/uptr.h"
#include "math/unit/time.h"
#include "egfx/imgui/texture.h"
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
		class game_mode;

		struct mp_portrait
		{
			std::string name;
			egfx::imgui::texture tex_a;
			egfx::imgui::texture tex_b;
			egfx::imgui::texture tex_shadow;
		};

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

			uptr<game_mode> game;

			std::vector<mp_portrait> portraits;
			egfx::imgui::texture combat_background;

			application(SDL_Window& window, egfx::module& gfx_module, config const& program_config);
			~application();

		public:
			static constexpr math::seconds fixed_step{ 0.02f };

			static application& create_instance(SDL_Window& window, egfx::module& gfx_module, config const& program_config);
			static void destroy_instance() noexcept;
			[[nodiscard]] static application& get_instance();

			SDL_Window& get_main_window() const noexcept { return *window; }

			void run();

			void save_game_data();
			void load_game_data();

			std::span<m3::enemy_template> get_enemy_templates() noexcept;
			void add_enemy_template(m3::enemy_template const& t);

			std::span<m3::player_character_data> get_player_data() noexcept;

			void change_game_mode(uptr<game_mode> new_game_mode);

			std::span<mp_portrait const> get_portraits() const noexcept { return portraits; }
			egfx::imgui::texture const& get_combat_background() const noexcept { return combat_background; }

		private:
			void load_monster_pack();

			void process_events();
		};
	}
}
