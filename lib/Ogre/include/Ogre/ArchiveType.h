#pragma once

#include "Ogre/Prerequisites.h"

namespace ot::ogre::archive_type
{
	extern const string filesystem;
#if OGRE_NO_ZIP_ARCHIVE == 0
	extern const string zip;
	extern const string embedded_zip;
#endif
}