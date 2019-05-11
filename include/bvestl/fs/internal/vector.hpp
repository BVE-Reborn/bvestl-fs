#pragma once

#include <EASTL/vector.h>
#include <bvestl/polyalloc/polyalloc.hpp>

namespace bvestl::fs::internal {
	template <class T>
	using vector = eastl::vector<T, bvestl::polyalloc::allocator_handle>;
}
