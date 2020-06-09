#pragma once

#if defined(_MSC_VER)
#  define OT_COMPILER_MSVC 1
#endif

#if !defined(OT_COMPILER_MSVC)
#  define OT_COMPILER_MSVC 0
#endif