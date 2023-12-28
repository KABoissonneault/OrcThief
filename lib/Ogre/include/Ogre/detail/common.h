#pragma once

#include "core/compiler.h"

#if OT_COMPILER_MSVC
#  define OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN \
     __pragma(warning(push)) \
     __pragma(warning(disable:4100)) /* unreferenced formal parameter */ \
     __pragma(warning(disable:4127)) /* conditional expression is constant */ \
     __pragma(warning(disable:4244)) /* conversion from 'double' to 'Ogre::Real', possible loss of data */ \
     __pragma(warning(disable:4251)) /* standard type needs to have dll-interface to be used */ \
     __pragma(warning(disable:4324)) /* structure was padded due to alignment specifier */ \
     __pragma(warning(disable:4819)) /* file contains a character that cannot be represented in the current code page */ \
     __pragma(warning(disable:5033)) /* 'register' is no longer a supported storage class */
#  define OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END \
     __pragma(warning(pop))
#endif