#pragma once

#include "Core/compiler.h"

#if OT_COMPILER_MSVC
#  define OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN \
     __pragma(warning(push)) \
     __pragma(warning(disable:4244)) // conversion from 'double' to 'Ogre::Real', possible loss of data \ 
     __pragma(warning(disable:4324)) // structure was padded due to alignment specifier
#  define OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END \
     __pragma(warning(pop))
#endif