#pragma once

#include "Ogre/detail/common.h"

#include <algorithm>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_BEGIN
#include <OgreMemoryAllocatorConfig.h>
OT_DETAIL_OGRE_EXTERNAL_INCLUDE_END

#include <cassert>
#include <type_traits>

namespace ot::ogre
{
	template<typename T>
	struct deleter
	{
		void operator()(T* t) const noexcept 
		{
			OGRE_DELETE t;
		}
	};

	template<typename T>
	using unique_ptr = std::unique_ptr<T, deleter<T>>;

	template<typename T, typename... Args>
	unique_ptr<T> make_unique(Args&&... args)
	{
		static_assert(std::is_base_of_v<Ogre::AllocatedObject, T>, "This function is for Ogre types");
		return unique_ptr<T>{OGRE_NEW T(std::forward<Args>(args)...) };
	}

	template<Ogre::MemoryCategory Category>
	struct simd_deleter
	{
		void operator()(void* p) const noexcept
		{
			OGRE_FREE_SIMD(p, Category);
		}
	};

	using unique_resource_mem = std::unique_ptr<void, simd_deleter<Ogre::MEMCATEGORY_RESOURCE>>;
	using unique_geometry_mem = std::unique_ptr<void, simd_deleter<Ogre::MEMCATEGORY_GEOMETRY>>;

	[[nodiscard]] inline unique_resource_mem allocate_resource(size_t bytes_n)
	{
		return unique_resource_mem(OGRE_MALLOC_SIMD(bytes_n, Ogre::MEMCATEGORY_RESOURCE));
	}

	[[nodiscard]] inline unique_geometry_mem allocate_geometry(size_t bytes_n)
	{
		auto const ptr = OGRE_MALLOC_SIMD(bytes_n, Ogre::MEMCATEGORY_GEOMETRY);
		assert(ptr != nullptr);
		return unique_geometry_mem(ptr);
	}
}