#pragma once

#include "compiler.h"

#if OT_COMPILER_MSVC
#	define OT_UNREACHABLE() __assume(0)
#else
#	define OT_UNREACHABLE()
#endif
