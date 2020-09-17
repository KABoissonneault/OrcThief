#pragma once

#include "core/compiler.h"

#if OT_COMPILER_MSVC
#define OT_BOOST_INCLUDE_BEGIN \
	__pragma(warning(push)) \
	__pragma(warning(disable:4100)) /* unreferenced formal parameter */ 
#define OT_BOOST_INCLUDE_END \
	__pragma(warning(pop))
#else
#define OT_BOOST_INCLUDE_BEGIN
#define OT_BOOST_INCLUDE_END
#endif