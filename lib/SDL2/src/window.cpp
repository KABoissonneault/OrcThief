#include "SDL2/window.h"

#include <SDL_video.h>

namespace ot::sdl 
{
    void window_delete::operator()(SDL_Window* p) const noexcept 
    {
        if(p != nullptr) 
            SDL_DestroyWindow(p);
    }
}