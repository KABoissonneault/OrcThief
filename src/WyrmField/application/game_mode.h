#pragma once

#include "core/uptr.h"
#include "math/unit/time.h"

union SDL_Event;

namespace ot::wf
{
	class application;

	class game_mode
	{
	public:
		virtual ~game_mode() = 0;
		virtual bool handle_hud_input(SDL_Event const& e) = 0;
		virtual void update(math::seconds dt) = 0;
		virtual void draw() = 0;
	};

	uptr<game_mode> get_play_mode(application& app);
	uptr<game_mode> get_combat_mode(application& app);
}