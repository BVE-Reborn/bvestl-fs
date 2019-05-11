#pragma once

#include "bvestl/fs/api.hpp"
#include <bvestl/polyalloc/polyalloc.hpp>

namespace bvestl::fs {
#ifdef BVESTL_FS_DISABLE_GLOBAL_ALLOCATOR
#	define BVESTL_FS_GET_GLOBAL_ALLOC
#else
#	define BVESTL_FS_GET_GLOBAL_ALLOC = get_global_allocator()
#endif
	BVESTL_FS_EXPORT void set_global_allocator(const bvestl::polyalloc::allocator_handle& handle);
	BVESTL_FS_EXPORT bvestl::polyalloc::allocator_handle get_global_allocator();
} // namespace bvestl::fs
