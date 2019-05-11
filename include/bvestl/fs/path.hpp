/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS

    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "bvestl/fs/allocation.hpp"
#include "bvestl/fs/api.hpp"
#include "bvestl/fs/fwd.hpp"
#include "bvestl/fs/internal/string.hpp"
#include "bvestl/fs/internal/vector.hpp"
#include <EABase/config/eaplatform.h>
#include <EASTL/optional.h>
#include <cinttypes>
#include <iosfwd>

namespace bvestl::fs {
	/**
	 * \brief Simple class for manipulating paths on Linux/Windows/Mac OS
	 *
	 * This class is just a temporary workaround to avoid the heavy boost
	 * dependency until boost::filesystem is integrated into the standard template
	 * library at some point in the future.
	 */
	class BVESTL_FS_EXPORT path {
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
		explicit path(bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {}

		path(const path& path) = default;
		path(path&& path) = default;
		path& operator=(const path& path) = default;
		path& operator=(path&& path) = default;

		explicit path(const char* string, bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {
			set(internal::string(string, handle), path_type::native_path, handle);
		}
		explicit path(const internal::string& string, bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {
			set(string, path_type::native_path, handle);
		}

		// Windows Constructors impl
#if defined(EA_PLATFORM_WINDOWS)
		path(const wchar_t* wstring, bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {
			set(internal::wstring(wstring, handle), handle);
		}
		path(const internal::wstring& wstring, bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) :
		    m_path(handle),
		    m_type(path_type::native_path),
		    m_absolute(false) {
			set(wstring, handle);
		}

		path& operator=(const internal::wstring& str) {
			set(str, m_path.get_allocator());
			return *this;
		}
#endif

		// Internal set-from string functions
		EA_FORCE_INLINE void set(internal::string const& str, bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) {
			set(str, path_type::native_path, handle);
		}
		void set(internal::string const& str, path_type type, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
#if defined(EA_PLATFORM_WINDOWS)
		void set(const internal::wstring& wstring, bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) {
			return set(wstring, path_type::native_path, handle);
		}
		void set(const internal::wstring& wstring, path_type type, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
#endif

		// Get-string functions
		EA_FORCE_INLINE internal::string str(bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) const {
			return str(path_type::native_path, handle);
		}
		internal::string str(path_type type, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;
#if defined(EA_PLATFORM_WINDOWS)
		EA_FORCE_INLINE internal::wstring wstr(bvestl::polyalloc::allocator_handle const handle BVESTL_FS_GET_GLOBAL_ALLOC) const {
			return wstr(path_type::native_path, handle);
		}
		internal::wstring wstr(path_type type, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;
#endif

		bool empty() const { return m_path.empty(); }
		size_t length() const { return m_path.size(); }
		internal::string filename(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;
		internal::string extension(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;

		size_t file_size(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;
		bool file_exists(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;

		bool is_absolute() const { return m_absolute; }
		bool is_directory(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;
		bool is_file(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;

		path make_absolute(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;
		path parent_path(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC) const;

		// Comparison Operators
		bool operator==(const path& p) const { return p.m_path == m_path; }
		bool operator!=(const path& p) const { return p.m_path != m_path; }

		// Modification Operators
		path operator/(const path& other) const;

		// Friend
		friend std::ostream& operator<<(std::ostream&, const path&);

#if defined(EA_PLATFORM_WINDOWS)
		static const size_t MAX_PATH_WINDOWS = 32767;
#endif
		static const size_t MAX_PATH_WINDOWS_LEGACY = 260;

	  protected:
		static internal::vector<internal::string> tokenize(const internal::string& string,
		                                                   const internal::string& deliminator,
		                                                   bvestl::polyalloc::allocator_handle handle);

		internal::vector<internal::string> m_path;
		path_type m_type;
		bool m_absolute;
	};

	// Utility
	BVESTL_FS_EXPORT path cwd(bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);

	BVESTL_FS_EXPORT bool create_directory(const path& p, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
	BVESTL_FS_EXPORT bool create_directory_recursive(const path& p, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
	BVESTL_FS_EXPORT bool remove_directory(const path& p, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
	BVESTL_FS_EXPORT bool remove_directory_recursive(const path& p, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
	BVESTL_FS_EXPORT bool remove_file(const path& p, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);
	BVESTL_FS_EXPORT bool resize_file(const path& p, size_t target_length, bvestl::polyalloc::allocator_handle handle BVESTL_FS_GET_GLOBAL_ALLOC);

	// Printing
	BVESTL_FS_EXPORT std::ostream& operator<<(std::ostream& os, const path& path);

} // namespace bvestl::fs
