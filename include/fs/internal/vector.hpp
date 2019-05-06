#pragma once

#include <EASTL/vector.h>

namespace fs::internal {
	template <class T>
	using vector = eastl::vector<T, eastl::polyalloc::allocator_handle>;
}
