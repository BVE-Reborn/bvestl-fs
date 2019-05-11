#pragma once

#include <bvestl/polyalloc/polyalloc.hpp>
#include <EASTL/string.h>

namespace bvestl::fs::internal {
	using string = eastl::basic_string<char, bvestl::polyalloc::allocator_handle>;
	using wstring = eastl::basic_string<wchar_t, bvestl::polyalloc::allocator_handle>;

	inline string substr(string const& str, std::size_t start, bvestl::polyalloc::allocator_handle handle) {
		return string(str.begin() + start, str.end(), handle);
	}
	inline string substr(string const& str, std::size_t start, std::size_t count, bvestl::polyalloc::allocator_handle handle) {
		return string(str.begin() + start, str.begin() + start + count, handle);
	}
	inline wstring substr(wstring const& str, std::size_t start, bvestl::polyalloc::allocator_handle handle) {
		return wstring(str.begin() + start, str.end(), handle);
	}
	inline wstring substr(wstring const& str, std::size_t start, std::size_t count, bvestl::polyalloc::allocator_handle handle) {
		return wstring(str.begin() + start, str.begin() + start + count, handle);
	}
} // namespace bvestl::fs::internal
