#pragma once

#include <cstdio>

#define OT_SDL_FAILURE(str) \
    std::printf("Unhandled error at %s:%d (%s): %s", __FILE__, __LINE__, str, SDL_GetError()); \
    std::abort() 
#define OT_SDL_FAILURE_IF(expr) \
        if(expr) { \
            OT_SDL_FAILURE(#expr); \
        } sizeof(0)
#define OT_SDL_FAILURE_IF_MSG(expr, msg) \
        if(expr) { \
            OT_SDL_FAILURE(msg); \
        } sizeof(0)
#define OT_SDL_ENSURE(expr) \
    if(auto const error = expr; error < 0) { \
        OT_SDL_FAILURE(#expr); \
    } sizeof(0)    