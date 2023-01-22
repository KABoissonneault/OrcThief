#include "application/game_mode.h"

#include "application/application.h"
#include "application/ui.h"

#include <SDL_events.h>

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
						app->change_game_mode(get_combat_mode(*app));
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
	}

	uptr<game_mode> get_play_mode(application& app)
	{
		return make_unique<play_mode>(app);
	}
}