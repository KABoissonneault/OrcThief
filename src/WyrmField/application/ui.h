#pragma once

#include <span>

struct SDL_Window;

namespace ot::wf
{
	namespace m3
	{
		struct character_data;
	}

	namespace ui
	{
		void draw_player_vitals(SDL_Window& window, std::span<m3::character_data const> player_data);
		void draw_player_vitals_content(std::span<m3::character_data const> player_data, float column_size);
		void draw_player_sheet(SDL_Window& window, m3::character_data const& player);
		void draw_player_sheet_content(m3::character_data const& player);
	}
}
