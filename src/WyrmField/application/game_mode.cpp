#include "application/game_mode.h"

#include "application/application.h"
#include "application/ui.h"

#include "core/directive.h"
#include "math/ops.h"

#include "Ogre/Image2.h"
#include "Ogre/ResourceGroupManager.h"
#include "Ogre/Root.h"
#include "Ogre/TextureGpuManager.h"

#include <SDL_events.h>
#include <imgui.h>
#include <optional>
#include <string_view>
#include <ranges>

namespace ot::wf
{
	namespace
	{
		class play_mode : public game_mode
		{
			application* app;

			int open_player_sheet = -1;

		public:
			play_mode(application& app)
				: app(&app)
			{

			}

			virtual bool handle_hud_input(SDL_Event const& e) override;
			virtual void update(math::seconds dt) override;
			virtual void draw() override;
		};

		enum class combat_action
		{
			attack,
			block,
			cast,
			examine,			
		};

		char const* to_string(combat_action action)
		{
			switch (action)
			{
			case combat_action::attack: return "Attack";
			case combat_action::block: return "Block";
			case combat_action::cast: return "Cast";
			case combat_action::examine: return "Examine";
			}
			OT_UNREACHABLE();
		}

		char get_action_hotkey(combat_action action)
		{
			switch (action)
			{
			case combat_action::attack: return 'A';
			case combat_action::block: return 'B';
			case combat_action::cast: return 'C';
			case combat_action::examine: return 'X';
			}
			OT_UNREACHABLE();
		}

		std::optional<combat_action> get_action(SDL_Keycode sym)
		{
			switch (sym)
			{
			case SDLK_a: return combat_action::attack;
			case SDLK_b: return combat_action::block;
			case SDLK_c: return combat_action::cast;
			case SDLK_x: return combat_action::examine;
			}
		}

		class combat_mode : public game_mode
		{
			application* app;

			int open_player_sheet = -1;
			math::seconds delay_buffer{0.f};
			std::vector<m3::enemy_character_data> enemies;

			int unit_turn;
			std::vector<std::string> combat_log;

		public:
			combat_mode(application& a)
				: app(&a)
			{
				m3::enemy_character_data& test_enemy1 = enemies.emplace_back();
				test_enemy1.base_template = app->get_enemy_templates()[0];
				test_enemy1.vitals = m3::generate_initial_vitals(test_enemy1.base_template.attributes);
				test_enemy1.count = 3;

				m3::enemy_character_data& test_enemy2 = enemies.emplace_back();
				test_enemy2.base_template = app->get_enemy_templates()[0];
				test_enemy2.vitals = m3::generate_initial_vitals(test_enemy2.base_template.attributes);
				test_enemy2.count = 2;

				m3::enemy_character_data& test_enemy3 = enemies.emplace_back();
				test_enemy3.base_template = app->get_enemy_templates()[0];
				test_enemy3.vitals = m3::generate_initial_vitals(test_enemy3.base_template.attributes);
				test_enemy3.count = 1;

				unit_turn = 0;
			}

			virtual bool handle_hud_input(SDL_Event const& e) override;
			virtual void update(math::seconds dt) override;
			virtual void draw() override;
		};

		bool play_mode::handle_hud_input(SDL_Event const& e)
		{
			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_6)
				{
					int const open_index = e.key.keysym.sym - SDLK_1;
					auto const player_data = app->get_player_data();
					if (open_index < player_data.size())
					{
						open_player_sheet = open_index;
					}
					return true;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					if (open_player_sheet != -1)
					{
						open_player_sheet = -1;
					}
					return true;
				}
				else if (e.key.keysym.sym == SDLK_HASH)
				{
					if (open_player_sheet == -1)
					{
						app->change_game_mode(make_unique<combat_mode>(*app));
					}
					return true;
				}
			}

			return false;
		}

		void play_mode::update(math::seconds dt)
		{
			(void)dt;
		}

		void play_mode::draw()
		{
			auto const player_data = app->get_player_data();
			if (open_player_sheet != -1)
			{
				ui::draw_player_sheet(app->get_main_window(), player_data[open_player_sheet]);
			}

			ui::draw_player_vitals(app->get_main_window(), player_data);
		}

		bool combat_mode::handle_hud_input(SDL_Event const& e)
		{
			// Use LCTRL to skip delay
			using namespace math::literals;
			if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_LCTRL)
			{
				delay_buffer = 0._s;
				return true;
			}
			
			// Otherwise, wait on delay
			if (delay_buffer > 0._s)
				return false;

			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_6)
				{
					int const open_index = e.key.keysym.sym - SDLK_1;
					auto const player_data = app->get_player_data();
					if (open_index < player_data.size())
					{
						open_player_sheet = open_index;
					}
					return true;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					if (open_player_sheet != -1)
					{
						open_player_sheet = -1;
					}
				}
			}

			return false;
		}

		void combat_mode::update(math::seconds dt)
		{
			using namespace math::literals;
			delay_buffer = math::max_value(delay_buffer - dt, 0._s);
		}

		void combat_mode::draw()
		{
			auto const player_data = app->get_player_data();
			if (open_player_sheet != -1)
			{
				ui::draw_player_sheet(app->get_main_window(), player_data[open_player_sheet]);
				ui::draw_player_vitals(app->get_main_window(), player_data);
			}
			else
			{
				int window_width, window_height;
				SDL_GetWindowSize(&app->get_main_window(), &window_width, &window_height);

				ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
				ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
				ImGui::SetNextWindowBgAlpha(1.0f);
				if (ImGui::Begin("##CombatScreen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
				{
					egfx::imgui::texture const& background = app->get_combat_background();
					float const background_upscale = 4.f;

					ImVec2 const initial_available_content = ImGui::GetContentRegionAvail();
					ImVec2 const viewport_size(background.get_width() * background_upscale, background.get_height() * background_upscale);
					if (ImGui::BeginChild("##CombatViewport", viewport_size, true /*border*/, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration))
					{						
						ImVec2 const space = ImGui::GetContentRegionAvail();

						ImGui::Image(background.get_texture_id(), space);

						auto const portraits = app->get_portraits();
						
						float const denominator = (int)enemies.size() + 1;

						for (size_t i = 0; i < enemies.size(); ++i)
						{
							m3::enemy_character_data const& e = enemies[i];
							auto const it_found = std::ranges::find(portraits, e.base_template.portrait, &mp_portrait::name);
							if (it_found == portraits.end())
								continue;

							mp_portrait const& portrait = *it_found;
							float const portrait_width = portrait.tex_a.get_width();
							float const portrait_height = portrait.tex_a.get_height();

							float const horizontal_dist = (i + 1.f) / denominator;
							ImVec2 const enemy_pos = ImVec2(space.x * horizontal_dist - portrait_width * 0.5f, space.y * 0.75f - portrait_height * 0.5f);

							auto draw_at_pos = [enemy_pos, &portrait](float local_pos_x, float local_pos_y)
							{
								ImVec2 const draw_pos = ImVec2(enemy_pos.x + local_pos_x, enemy_pos.y + local_pos_y);
								ImGui::SetCursorPos(draw_pos);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
								ImGui::Image(portrait.tex_shadow.get_texture_id(), ImVec2(portrait.tex_shadow.get_width() * 2.f, portrait.tex_shadow.get_height() * 2.f));
								ImGui::PopStyleVar();
								ImGui::SetCursorPos(draw_pos);
								ImGui::Image(portrait.tex_a.get_texture_id(), ImVec2(portrait.tex_a.get_width() * 2.f, portrait.tex_a.get_height() * 2.f));
							};							

							if (e.count > 2)
							{
								draw_at_pos(-portrait_width * 0.75f, -portrait_height * 0.5f);
								draw_at_pos(portrait_width * 0.75f, -portrait_height * 0.5f);
								draw_at_pos(0.f, 0.f);
							}
							else if (e.count == 2)
							{
								draw_at_pos(-portrait_width * 0.75f, 0.f);
								draw_at_pos(portrait_width * 0.75f, 0.f);
							}
							else
							{
								draw_at_pos(0.f, 0.f);
							}							
						}
					}
					ImGui::EndChild();

					ImGui::SameLine();
					ImVec2 const enemy_list_size(0, viewport_size.y);
					if (ImGui::BeginChild("##EnemyList", enemy_list_size, true /*border*/))
					{
						for (size_t i = 0; i < enemies.size(); ++i)
						{
							m3::enemy_character_data const& e = enemies[i];

							ImGui::NewLine();

							char const hotkey = 'A' + (char)i;
							if (e.count > 1)
							{
								ImGui::Text("%c) %s (%d)", hotkey, e.base_template.name.c_str(), e.count);
							}
							else
							{
								ImGui::Text("%c) %s", hotkey, e.base_template.name.c_str());
							}
						}
					}
					ImGui::EndChild();

					ImVec2 const log_size(viewport_size.x, 0.f);
					if (ImGui::BeginChild("##Log", log_size, true /*border*/))
					{
						if (unit_turn >= 0 && unit_turn < player_data.size())
						{
							m3::player_character_data const& player = player_data[unit_turn];

							ImGui::Text("Turn: %s", player.name.c_str());

							std::vector<combat_action> available_actions{ combat_action::attack, combat_action::block };
							if (player.skills.astrology > 0 || player.skills.medecine > 0 || player.skills.rhetoric > 0)
							{
								available_actions.push_back(combat_action::cast);
							}
							available_actions.push_back(combat_action::examine);

							size_t const line_count = math::min_value(available_actions.size(), 4);
							for (size_t i = 0; i < line_count; ++i)
							{
								auto const first_action = available_actions[i];
								ImGui::Text("'%c' %s", get_action_hotkey(first_action), to_string(first_action));
							}
						}
					}
					ImGui::EndChild();

					ImGui::SameLine();
					ImVec2 const player_vitals(0.f, 0.f);
					if (ImGui::BeginChild("##PlayerVitals", player_vitals, true /*border*/))
					{
						float const column_size = ImGui::GetContentRegionAvail().x / (player_data.size() + 1);
						ui::draw_player_vitals_content(player_data, column_size);
					}
					ImGui::EndChild();

				}
				ImGui::End();
			}
		}
	}

	game_mode::~game_mode() = default;

	uptr<game_mode> get_play_mode(application& app)
	{
		return make_unique<play_mode>(app);
	}
}
