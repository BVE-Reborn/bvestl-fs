/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS

    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "fs/allocation.hpp"
#include "fs/api.hpp"
#include "fs/fwd.hpp"
#include "fs/internal/string.hpp"
#include "fs/internal/vector.hpp"
#include <EABase/config/eaplatform.h>
#include <EASTL/optional.h>
#include <cinttypes>
#include <iosfwd>

namespace fs {
	/**
	 * \brief Simple class for manipulating paths on Linux/Windows/Mac OS
	 *
	 * This class is just a temporary workaround to avoid the heavy boost
	 * dependency until boost::filesystem is integrated into the standard template
	 * library at some point in the future.
	 */
	class LIBFS_EXPORT path {
	  public:
		enum class path_type : std::uint8_t {
			windows_path = 0,
			posix_path = 1,
#if defined(EA_PLATFORM_WINDOWS)
			native_path = windows_path
#else
			native_path = posix_path
#endif
		};

		// Constructors
		explicit path(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {}

		path(const path& path) = default;
		path(path&& path) = default;
		path& operator=(const path& path) = default;
		path& operator=(path&& path) = default;

		explicit path(const char* string, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {
			set(internal::string(string, handle), path_type::native_path, handle);
		}
		explicit path(const internal::string& string, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {
			set(string, path_type::native_path, handle);
		}

#if defined(EA_PLATFORM_WINDOWS)
		// Windows Constructors impl
		path(const wchar_t* wstring, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) {
			set(internal::wstring(wstring, handle), handle);
		}
		path(const internal::wstring& wstring, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) { set(wstring, handle); }

		path& operator=(const internal::wstring& str) {
			set(str);
			return *this;
		}
#endif

		// Internal set-from string functions
		EA_FORCE_INLINE void set(internal::string const& str, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) {
			set(str, path_type::native_path, handle);
		}
		void set(internal::string const& str, path_type type, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC);
#if defined(EA_PLATFORM_WINDOWS)
		void set(const internal::wstring& wstring, path_type type = native_path);
#endif

		// Get-string functions
		EA_FORCE_INLINE internal::string str(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const {
			return str(path_type::native_path, handle);
		}
		internal::string str(path_type type, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;
#if defined(EA_PLATFORM_WINDOWS)
		std::wstring wstr(path_type type = native_path) const;
#endif

		bool empty() const { return m_path.empty(); }
		size_t length() const { return m_path.size(); }
		internal::string filename(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;
		internal::string extension(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;

		size_t file_size(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;
		bool file_exists(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;

		bool is_absolute() const { return m_absolute; }
		bool is_directory(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;
		bool is_file(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;

		path make_absolute(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;
		path parent_path(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC) const;

		// Comparison Operators
		bool operator==(const path& p) const { return p.m_path == m_path; }
		bool operator!=(const path& p) const { return p.m_path != m_path; }

		// Modification Operators
		path operator/(const path& other) const;

		// Friend
		friend std::ostream& operator<<(std::ostream&, const path&);

	  protected:
		static internal::vector<internal::string> tokenize(const internal::string& string,
		                                                   const internal::string& delim,
		                                                   eastl::polyalloc::allocator_handle handle);

#if defined(EA_PLATFORM_WINDOWS)
		static const size_t MAX_PATH_WINDOWS = 32767;
#endif
		static const size_t MAX_PATH_WINDOWS_LEGACY = 260;
		internal::vector<internal::string> m_path;
		path_type m_type;
		bool m_absolute;
	};

	// Utility
	LIBFS_EXPORT path cwd(eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC);

	LIBFS_EXPORT bool create_directory(const path& p, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC);
	LIBFS_EXPORT bool create_directories(const path& p, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC);
	LIBFS_EXPORT bool remove_file(const path& p, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC);
	LIBFS_EXPORT bool resize_file(const path& p, size_t target_length, eastl::polyalloc::allocator_handle handle LIBFS_GET_GLOBAL_ALLOC);

	// Printing
	LIBFS_EXPORT std::ostream& operator<<(std::ostream& os, const path& path);

} // namespace fs
