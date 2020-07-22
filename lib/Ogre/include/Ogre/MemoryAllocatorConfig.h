#pragma once

#include "Ogre/detail/common.h"

OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN
#include <OgreMemoryAllocatorConfig.h>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END

#include <cassert>

namespace ot::ogre
{
	template<Ogre::MemoryCategory Category>
	struct simd_deleter
	{
		void operator()(void* p) const noexcept
		{
			OGRE_FREE_SIMD(p, Category);
		}
	};

	using unique_geometry_mem = std::unique_ptr<void, simd_deleter<Ogre::MEMCATEGORY_GEOMETRY>>;

	inline [[nodiscard]] unique_geometry_mem allocate_geometry(size_t bytes_n)
	{
		auto const ptr = OGRE_MALLOC_SIMD(bytes_n, Ogre::MEMCATEGORY_GEOMETRY);
		assert(ptr != nullptr);
		return unique_geometry_mem(ptr);
	}
}