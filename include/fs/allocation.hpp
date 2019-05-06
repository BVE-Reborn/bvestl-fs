#pragma once

#include "fs/api.hpp"
#include <EASTL/polyalloc/polyalloc.h>

namespace fs {
#ifdef LIBFS_DISABLE_GLOBAL_ALLOCATOR
#	define LIBFS_GET_GLOBAL_ALLOC
#else
#	define LIBFS_GET_GLOBAL_ALLOC = get_global_allocator()
#endif
	LIBFS_EXPORT void set_global_allocator(const eastl::polyalloc::allocator_handle& handle);
	LIBFS_EXPORT eastl::polyalloc::allocator_handle get_global_allocator();
} // namespace fs
