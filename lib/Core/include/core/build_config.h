#pragma once

#if defined(_DEBUG)
#define OT_BUILD_DEBUG 1
#define OT_BUILD_RELEASE 0
#elif defined(NDEBUG)
#define OT_BUILD_DEBUG 0
#define OT_BUILD_RELEASE 1
#else
#error "Undefined build config"
#endif
