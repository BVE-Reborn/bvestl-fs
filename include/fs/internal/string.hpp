#pragma once

#include <EASTL/polyalloc/polyalloc.h>
#include <EASTL/string.h>

namespace fs::internal {
	using string = eastl::basic_string<char, eastl::polyalloc::allocator_handle>;
	using wstring = eastl::basic_string<wchar_t, eastl::polyalloc::allocator_handle>;

	inline string substr(string const& str, std::size_t start, eastl::polyalloc::allocator_handle handle) {
		return string(str.begin() + start, str.end(), handle);
	}
	inline string substr(string const& str, std::size_t start, std::size_t count, eastl::polyalloc::allocator_handle handle) {
		return string(str.begin() + start, str.begin() + start + count, handle);
	}
} // namespace fs::internal
