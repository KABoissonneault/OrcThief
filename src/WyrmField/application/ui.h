#pragma once

#include <span>

struct SDL_Window;

namespace ot::wf
{
	namespace m3
	{
		struct player_character_data;
		struct enemy_character_data;
	}

	namespace ui
	{
		void draw_player_vitals(SDL_Window& window, std::span<m3::player_character_data const> player_data);
		void draw_player_sheet(SDL_Window& window, m3::player_character_data const& player);
	}
}
