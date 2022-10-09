#pragma once

#include <memory>

struct SDL_Window;
struct SDL_Renderer;

namespace ot::sdl 
{
    struct window_delete 
    {
        void operator()(SDL_Window* p) const noexcept;
    };

    using unique_window = std::unique_ptr<SDL_Window, window_delete>;
}
