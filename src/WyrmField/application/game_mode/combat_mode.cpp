#include "application/game_mode.h"

#include "application/application.h"
#include "application/ui.h"

#include "core/directive.h"
#include "math/ops.h"

#include <SDL_events.h>
#include <imgui.h>

namespace ot::wf
{
	namespace
	{
		enum class combat_action
		{
			none,
			attack, // attack engaged enemies
			block, // stay back
			cast, // use an ability
			defend, // guard another character
			engage, // Embark in melee combat
			focus, // target a specific unit
			retreat, // move back a row
			examine, // not a real action - gets a description of the enemy
		};

		char const* to_string(combat_action action)
		{
			switch (action)
			{
			case combat_action::attack: return "Attack";
			case combat_action::block: return "Block";
			case combat_action::cast: return "Cast";
			case combat_action::defend: return "Defend";
			case combat_action::engage: return "Engage";
			case combat_action::retreat: return "Retreat";
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
			case combat_action::defend: return 'D';
			case combat_action::engage: return 'E';
			case combat_action::retreat: return 'R';
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
			case SDLK_d: return combat_action::defend;
			case SDLK_e: return combat_action::engage;
			case SDLK_r: return combat_action::retreat;
			case SDLK_x: return combat_action::examine;
			}

			return std::nullopt;
		}

		enum class row_position
		{
			retreat, // Own retreat row
			back, // Own back row
			melee, // Middle row
			assault, // Enemy back row
			chase // Enemy retreat row
		};

		enum class enemy_attitude
		{
			passive, // not feeling threatened
			prudent, // watching for threat
			aggressive, // attacking threat
			retreating, // fleeing threat
			petrified, // paralyzed by fear
		};

		const size_t enemy_attitude_count = static_cast<size_t>(enemy_attitude::petrified) + 1;

		struct unit_state
		{
			row_position position;
			combat_action action;
			std::vector<size_t> engaged_characters;
			double initiative;

			virtual m3::character_data& get_character_data() noexcept = 0;
		};

		struct enemy_state : unit_state
		{
			m3::enemy_template const* base_template;
			m3::character_data enemy_data;
			enemy_attitude attitude;
			int count;

			virtual m3::character_data& get_character_data() noexcept override { return enemy_data; }
		};

		struct player_state : unit_state
		{
			m3::character_data* player_data;

			virtual m3::character_data& get_character_data() noexcept override { return *player_data; }
		};

		class combat_mode : public game_mode
		{
			application* app;

			std::vector<enemy_state> enemies;
			std::vector<player_state> players;

			int unit_turn;
			int open_sheet_index = -1;
			std::vector<std::string> combat_log;
			std::vector<std::string> log_buffer;

			math::seconds delay_buffer{ 0.f };
			static constexpr math::seconds log_delay{ 1.f };

			bool turn_over = false;

		public:
			combat_mode(application& a)
				: app(&a)
			{
				auto make_enemy = [this](size_t template_index) -> enemy_state&
				{
					enemy_state& test_enemy = enemies.emplace_back();
					test_enemy.base_template = &app->get_enemy_templates()[template_index];
					test_enemy.enemy_data.name = test_enemy.base_template->name;
					test_enemy.enemy_data.attributes = test_enemy.base_template->attributes;
					test_enemy.enemy_data.vitals = m3::generate_initial_vitals(test_enemy.enemy_data.attributes);
					return test_enemy;
				};

				enemy_state& test_enemy1 = make_enemy(0);
				test_enemy1.count = 3;
				test_enemy1.position = row_position::back;

				enemy_state& test_enemy2 = make_enemy(0);
				test_enemy2.count = 2;
				test_enemy2.position = row_position::back;

				enemy_state& test_enemy3 = make_enemy(0);
				test_enemy3.count = 1;
				test_enemy3.position = row_position::back;

				for (m3::character_data& player_data : app->get_player_data())
				{
					player_state& p = players.emplace_back();
					p.player_data = &player_data;
					p.position = row_position::back;
				}

				combat_log.push_back("Vermin has appeared!");

				unit_turn = 0;
			}

			virtual bool handle_hud_input(SDL_Event const& e) override;
			virtual void update(math::seconds dt) override;
			virtual void draw() override;

		private:
			std::vector<unit_state*> get_units();

			void push_combat_log(std::string&& message);
			void advance_message_print();

			void resolve_initial_attitude();
			void choose_enemy_action();

			void advance_turn();
			void resolve_round();

			void handle_player_no_action(SDL_KeyboardEvent const& k);
			void handle_player_cast(SDL_KeyboardEvent const& k);
			void handle_player_examine(SDL_KeyboardEvent const& k);

			void draw_enemy_sheet();

			std::vector<combat_action> get_available_actions() const;
		};

		bool combat_mode::handle_hud_input(SDL_Event const& e)
		{
			if (e.type == SDL_KEYDOWN)
			{
				if (unit_turn >= 0)
				{
					combat_action const player_action = players[unit_turn].action;
					if (player_action == combat_action::none)
					{
						handle_player_no_action(e.key);
					}
					else if (player_action == combat_action::examine)
					{
						handle_player_examine(e.key);
					}
					else if (player_action == combat_action::cast)
					{
						handle_player_cast(e.key);
					}

					return true;
				}
				else
				{
					if (e.key.repeat == 0 || e.key.keysym.scancode == SDL_SCANCODE_LCTRL)
					{
						if (!log_buffer.empty() || turn_over)
						{
							advance_message_print();
						}
						else
						{
							combat_log.clear();
							unit_turn = 0;
						}
					}
				}
			}

			return false;
		}

		void combat_mode::handle_player_no_action(SDL_KeyboardEvent const& k)
		{
			if (open_sheet_index == -1)
			{
				if (k.keysym.sym >= SDLK_1 && k.keysym.sym <= SDLK_6)
				{
					int const open_index = k.keysym.sym - SDLK_1;
					auto const player_data = app->get_player_data();
					if (open_index < player_data.size())
					{
						open_sheet_index = open_index;
					}
				}
				else if (k.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					if (unit_turn > 0)
					{
						players[unit_turn - 1].action = combat_action::none;
						--unit_turn;
					}
				}
				else if (std::optional<combat_action> action = get_action(k.keysym.sym))
				{
					auto const available_actions = get_available_actions();

					auto const it_found = std::ranges::find(available_actions, *action);

					if (it_found != available_actions.end())
					{
						if (k.repeat == 0)
						{
							if (unit_turn >= 0)
							{
								auto const a = players[unit_turn].action = *action;

								if (a == combat_action::attack
									|| a == combat_action::block
									|| a == combat_action::engage
									|| a == combat_action::retreat)
								{
									advance_turn();
								}
							}
						}
					}
				}

			}
			else
			{
				if (k.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					if (open_sheet_index != -1)
					{
						open_sheet_index = -1;
					}
				}
			}
		}

		void combat_mode::handle_player_examine(SDL_KeyboardEvent const& k)
		{
			if (open_sheet_index < 0)
			{
				combat_action& player_action = players[unit_turn].action;
				if (k.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					player_action = combat_action::none;
				}
				else if (k.keysym.sym >= SDLK_a && k.keysym.sym <= SDLK_z)
				{
					size_t const enemy_index = k.keysym.sym - SDLK_a;
					if (enemy_index < enemies.size())
					{
						open_sheet_index = (int)enemy_index;
					}
				}
			}
			else
			{
				if (k.keysym.scancode == SDL_SCANCODE_ESCAPE)
				{
					open_sheet_index = -1;
				}
			}
		}

		void combat_mode::handle_player_cast(SDL_KeyboardEvent const& k)
		{
			combat_action& player_action = players[unit_turn].action;
			if (k.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				player_action = combat_action::none;
			}
		}

		std::vector<combat_action> combat_mode::get_available_actions() const
		{
			m3::character_data const& player = *players[unit_turn].player_data;
			std::vector<size_t> const& engaged_enemies = players[unit_turn].engaged_characters;
			row_position const position = players[unit_turn].position;

			std::vector<combat_action> available_actions;
			if (engaged_enemies.size() > 0)
				available_actions.push_back(combat_action::attack);
			available_actions.push_back(combat_action::block);

			if (player.skills.astrology > 0 || player.skills.medecine > 0 || player.skills.rhetoric > 0)
				available_actions.push_back(combat_action::cast);

			// available_actions.push_back(combat_action::defend);

			if (position == row_position::back)
				available_actions.push_back(combat_action::engage);

			available_actions.push_back(combat_action::retreat);
			available_actions.push_back(combat_action::examine);

			return available_actions;
		}

		void combat_mode::resolve_initial_attitude()
		{
			std::uniform_int_distribution<size_t> dist(0, enemy_attitude_count - 1);

			// Just go random for now
			auto const generate_attitude = [&dist, this]()
			{
				return static_cast<enemy_attitude>(dist(app->get_random_generator()));
			};

			for (enemy_state& enemy : enemies)
			{
				enemy.attitude = generate_attitude();
			}
		}

		void combat_mode::choose_enemy_action()
		{
			std::uniform_int_distribution d100(0, 99);
			auto const roll_d100 = [this, &d100]()
			{
				return d100(app->get_random_generator());
			};

			// Attitudes: passive, prudent, aggressive, retreating, petrified
			for (enemy_state& enemy : enemies)
			{
				size_t const attitude_index = static_cast<size_t>(enemy.attitude);
				std::vector<combat_action> options;

				auto const roll_chance = [attitude_index, &roll_d100, &options](int const(&values)[enemy_attitude_count], combat_action action) constexpr
				{
					if (values[attitude_index] != 0 && roll_d100() < values[attitude_index])
						options.push_back(action);
				};

				switch (enemy.position)
				{
				case row_position::retreat:
				{
					if (enemy.engaged_characters.size() > 0)
					{
						constexpr int none_chance[] = { 100, 50, 0, 25, 100 };
						roll_chance(none_chance, combat_action::none);

						constexpr int attack_chance[] = { 100, 100, 100, 50, 25 };
						roll_chance(attack_chance, combat_action::attack);

						constexpr int block_chance[] = { 100, 100, 0, 50, 25 };
						roll_chance(block_chance, combat_action::block);

						constexpr int flee_chance[] = { 100, 100, 0, 50, 25 };
						roll_chance(flee_chance, combat_action::retreat);
					}
					else
					{
						constexpr int none_chance[] = { 100, 50, 0, 25, 100 };
						roll_chance(none_chance, combat_action::none);

						constexpr int block_chance[] = { 0, 100, 0, 25, 20 };
						roll_chance(block_chance, combat_action::block);

						constexpr int engage_chance[] = { 20, 50, 100, 0, 0 };
						roll_chance(engage_chance, combat_action::engage);

						constexpr int flee_chance[] = { 50, 50, 0, 100, 25 };
						roll_chance(flee_chance, combat_action::retreat);
					}

					break;
				}

				case row_position::back:
				{
					if (enemy.engaged_characters.size() > 0)
					{
						constexpr int none_chance[] = { 100, 25, 0, 0, 100 };
						roll_chance(none_chance, combat_action::none);

						constexpr int attack_chance[] = { 100, 100, 100, 50, 25 };
						roll_chance(attack_chance, combat_action::attack);

						constexpr int block_chance[] = { 100, 100, 0, 50, 25 };
						roll_chance(block_chance, combat_action::block);

						constexpr int flee_chance[] = { 100, 100, 0, 50, 25 };
						roll_chance(flee_chance, combat_action::retreat);
					}
					else
					{
						constexpr int none_chance[] = { 100, 50, 0, 25, 100 };
						roll_chance(none_chance, combat_action::none);

						constexpr int block_chance[] = { 0, 100, 0, 25, 20 };
						roll_chance(block_chance, combat_action::block);

						constexpr int engage_chance[] = { 20, 20, 100, 0, 0 };
						roll_chance(engage_chance, combat_action::engage);

						constexpr int flee_chance[] = { 50, 50, 0, 100, 25 };
						roll_chance(flee_chance, combat_action::retreat);
					}

					break;
				}

				case row_position::melee:
					// TODO
					break;

				case row_position::assault:
					// TODO
					break;

				case row_position::chase:
					// TODO
					break;
				}

				if (options.size() == 0)
				{
					enemy.action = combat_action::none;
				}
				else if (options.size() == 1)
				{
					enemy.action = options[0];
				}
				else
				{
					std::uniform_int_distribution options_r(0, (int)options.size() - 1);
					enemy.action = options[options_r(app->get_random_generator())];
				}
			}
		}

		std::vector<unit_state*> combat_mode::get_units()
		{
			std::vector<unit_state*> units;
			units.reserve(players.size() + enemies.size());

			for (player_state& player : players)
			{
				units.push_back(&player);
			}

			for (enemy_state& enemy : enemies)
			{
				units.push_back(&enemy);
			}

			return units;
		}

		void combat_mode::push_combat_log(std::string&& message)
		{
			log_buffer.push_back(as_moveable(message));
		}

		void combat_mode::advance_message_print()
		{
			if (turn_over)
			{
				combat_log.push_back("");
				combat_log.push_back("Press a key to continue.");
				turn_over = false;
			}
			else
			{
				combat_log.push_back(as_moveable(log_buffer.back()));
				log_buffer.pop_back();

				if (log_buffer.empty())
				{
					turn_over = true;
				}

				delay_buffer = log_delay;
			}
		}

		void combat_mode::advance_turn()
		{
			++unit_turn;
			if (unit_turn >= app->get_player_data().size())
			{
				unit_turn = -1;
				resolve_round();
			}
		}

		void combat_mode::resolve_round()
		{
			// Decide enemy actions
			choose_enemy_action();

			std::vector<unit_state*> units = get_units();


			std::normal_distribution<double> init_roll(50.f, 15.f);

			for (unit_state* unit : units)
			{
				m3::character_data const& unit_data = unit->get_character_data();

				// Resolve Engage
				if (unit->action == combat_action::engage)
				{
					unit->position = row_position::melee;
				}

				// Roll Initiative
				unit->initiative = init_roll(app->get_random_generator()) + unit_data.attributes.agility;
			}

			std::ranges::sort(units, std::greater(), &unit_state::initiative);

			// DEBUG: print units actions
			for (unit_state* unit : units)
			{
				m3::character_data const& unit_data = unit->get_character_data();

				switch (unit->action)
				{
				case combat_action::none:
					break;

				case combat_action::attack:
					push_combat_log(std::format("{} attacks its target.", unit_data.name));
					break;

				case combat_action::block:
					push_combat_log(std::format("{} protects themself.", unit_data.name));
					break;

				case combat_action::cast:
					push_combat_log(std::format("{} casts a spell.", unit_data.name));
					break;

				case combat_action::defend:
					push_combat_log(std::format("{} defends an ally.", unit_data.name));
					break;

				case combat_action::engage:
					push_combat_log(std::format("{} engages in melee.", unit_data.name));
					break;

				case combat_action::focus:
					push_combat_log(std::format("{} focuses on a specific target.", unit_data.name));
					break;

				case combat_action::retreat:
					switch (unit->position)
					{
					case row_position::retreat:
						push_combat_log(std::format("{} retreats from battle.", unit_data.name));
						break;

					case row_position::back:
						push_combat_log(std::format("{} retreats to the edge of battle.", unit_data.name));
						break;

					case row_position::melee:
						push_combat_log(std::format("{} retreats from melee.", unit_data.name));
						break;

					case row_position::assault:
						push_combat_log(std::format("{} retreats from enemy lines.", unit_data.name));
						break;

					case row_position::chase:
						push_combat_log(std::format("{} retreats from chasing deserters.", unit_data.name));
						break;
					}
					break;

				case combat_action::examine:
					break;
				}

				unit->action = combat_action::none;
			}
		}

		void combat_mode::update(math::seconds dt)
		{
			using namespace math::literals;
			delay_buffer = math::max_value(delay_buffer - dt, 0._s);

			if (delay_buffer == 0._s)
			{
				if (!log_buffer.empty() || turn_over)
				{
					advance_message_print();
				}
			}
		}

		void combat_mode::draw()
		{
			int window_width, window_height;
			SDL_GetWindowSize(&app->get_main_window(), &window_width, &window_height);

			ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
			ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window_width), static_cast<float>(window_height)));
			ImGui::SetNextWindowBgAlpha(1.0f);
			if (ImGui::Begin("##CombatScreen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
			{
				egfx::imgui::texture const& background = app->get_combat_background();
				float const background_upscale = 4.f;

				ImVec2 const initial_available_content = ImGui::GetContentRegionAvail();
				ImVec2 const viewport_size(background.get_width() * background_upscale, background.get_height() * background_upscale);
				if (ImGui::BeginChild("##CombatViewport", viewport_size, true /*border*/, ImGuiWindowFlags_NoInputs))
				{
					ImVec2 const space = ImGui::GetContentRegionAvail();

					ImGui::Image(background.get_texture_id(), space);

					auto const portraits = app->get_portraits();

					float const denominator = static_cast<float>(enemies.size()) + 1.f;

					for (size_t i = 0; i < enemies.size(); ++i)
					{
						enemy_state const& e = enemies[i];
						auto const it_found = std::ranges::find(portraits, e.base_template->portrait, &mp_portrait::name);
						if (it_found == portraits.end())
							continue;

						mp_portrait const& portrait = *it_found;
						float const portrait_width = static_cast<float>(portrait.tex_a.get_width());
						float const portrait_height = static_cast<float>(portrait.tex_a.get_height());

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
				ImVec2 const topright_size(0, viewport_size.y);
				if (open_sheet_index < 0)
				{
					if (ImGui::BeginChild("##EnemyList", topright_size, true /*border*/, ImGuiWindowFlags_NoInputs))
					{
						for (size_t i = 0; i < enemies.size(); ++i)
						{
							enemy_state const& e = enemies[i];

							ImGui::NewLine();

							char const hotkey = 'A' + (char)i;
							if (e.count > 1)
							{
								ImGui::Text("%c) %s (%d)", hotkey, e.base_template->name.c_str(), e.count);
							}
							else
							{
								ImGui::Text("%c) %s", hotkey, e.base_template->name.c_str());
							}
						}
					}
					ImGui::EndChild();
				}
				else
				{
					combat_action const current_action = players[unit_turn].action;
					if (current_action == combat_action::none)
					{
						if (ImGui::BeginChild("##CharacterSheet", topright_size, true /*border*/, ImGuiWindowFlags_NoInputs))
						{
							m3::character_data const& sheet_player = *players[open_sheet_index].player_data;
							ui::draw_player_sheet_content(sheet_player);
						}
						ImGui::EndChild();
					}
					else if (current_action == combat_action::examine)
					{
						if (ImGui::BeginChild("##EnemySheet", topright_size, true, ImGuiWindowFlags_NoInputs))
						{
							draw_enemy_sheet();
						}
						ImGui::EndChild();
					}
				}

				ImVec2 const log_size(viewport_size.x, 0.f);
				if (ImGui::BeginChild("##Log", log_size, true /*border*/, ImGuiWindowFlags_NoInputs))
				{
					for (std::string const& line : combat_log)
					{
						ImGui::Text("%s", line.c_str());
					}
				}
				ImGui::EndChild();

				ImGui::SameLine();
				ImVec2 const player_info_size(0.f, 0.f);
				if (ImGui::BeginChild("##PlayerInfo", player_info_size, true /*border*/, ImGuiWindowFlags_NoInputs))
				{
					auto const player_data = app->get_player_data();
					float const column_size = ImGui::GetContentRegionAvail().x / (player_data.size() + 1);
					ui::draw_player_vitals_content(player_data, column_size);

					ImGui::NewLine();
					ImGui::Text("Action");

					size_t const player_count = math::min_value(player_data.size(), 6);
					for (int i = 0; i < player_count; ++i)
					{
						if (players[i].action != combat_action::none)
						{
							ImGui::SameLine(column_size * (i + 1.f));
							ImGui::Text("%s", to_string(players[i].action));
						}
					}

					ImGui::NewLine();
					ImGui::Separator();
					ImGui::NewLine();

					if (unit_turn >= 0 && unit_turn < player_data.size())
					{
						m3::character_data const& player = player_data[unit_turn];
						combat_action const current_action = players[unit_turn].action;

						if (open_sheet_index == -1)
						{
							if (current_action == combat_action::none)
							{
								ImGui::Text("Turn: %s", player.name.c_str());

								ImGui::NewLine();

								auto const available_actions = get_available_actions();

								size_t const max_line = 4;
								size_t const line_count = math::min_value(available_actions.size(), max_line);
								size_t const column_count = available_actions.size() / 4 + 1;
								ImGui::NewLine();
								for (size_t i = 0; i < line_count; ++i)
								{
									for (size_t j = 0; j < column_count; ++j)
									{
										size_t const action_index = max_line * j + i;
										if (action_index >= available_actions.size())
										{
											break;
										}

										combat_action const action = available_actions[action_index];

										ImGui::SameLine(128.f * j);
										ImGui::Text("'%c' %s", get_action_hotkey(action), to_string(action));
									}
									ImGui::NewLine();
								}
							}
							else if (current_action == combat_action::examine)
							{
								ImGui::Text("Examine?");

								for (int i = 0; i < enemies.size(); ++i)
								{
									ImGui::Text("%c) %s", 'A' + i, enemies[i].base_template->name.c_str());
								}
							}
						}
					}
				}
				ImGui::EndChild();

			}
			ImGui::End();
		}

		void combat_mode::draw_enemy_sheet()
		{
			enemy_state const& sheet_enemy = enemies[open_sheet_index];
			enemy_attitude const enemy_attitude = enemies[open_sheet_index].attitude;
			if (sheet_enemy.count > 1)
			{
				ImGui::Text("This is a group of %d %s.", sheet_enemy.count, sheet_enemy.base_template->name.c_str());
			}
			else
			{
				ImGui::Text("This is a %s.", sheet_enemy.base_template->name.c_str());
			}

			auto const portraits = app->get_portraits();
			auto const it_found = std::ranges::find(portraits, sheet_enemy.base_template->portrait, &mp_portrait::name);
			if (it_found != portraits.end())
			{
				ImGui::SameLine();
				ImVec2 const image_size(static_cast<float>(it_found->tex_a.get_width()), static_cast<float>(it_found->tex_a.get_height()));
				ImGui::Image(it_found->tex_a.get_texture_id(), image_size);
			}

			ImGui::NewLine();

			switch (enemy_attitude)
			{
			case enemy_attitude::passive: ImGui::Text("They seem uninterested to fight."); break;
			case enemy_attitude::prudent: ImGui::Text("They seem prepared to defend themselves."); break;
			case enemy_attitude::aggressive: ImGui::Text("They seem ready to jump in."); break;
			case enemy_attitude::retreating: ImGui::Text("They seem about to run away."); break;
			case enemy_attitude::petrified: ImGui::Text("They seem petrified by fear."); break;
			}
		}
	}

	uptr<game_mode> get_combat_mode(application& app)
	{
		return make_unique<combat_mode>(app);
	}
}