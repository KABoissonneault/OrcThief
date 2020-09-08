#include "input.h"

#include <SDL_mouse.h>

namespace ot::dedit::input
{
	void get_mouse_position(int& x, int& y)
	{
		SDL_GetMouseState(&x, &y);
	}
}