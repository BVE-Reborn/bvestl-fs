/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS

    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "fs/api.hpp"
#include "fs/fwd.hpp"
#include <EABase/config/eaplatform.h>
#include <cinttypes>
#include <string>
#include <vector>

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
		path() : m_type(path_type::native_path), m_absolute(false) {}

		path(const path& path) = default;
		path(path&& path) = default;
		path& operator=(const path& path) = default;
		path& operator=(path&& path) = default;

		explicit path(const char* string) { set(string); }
		explicit path(const std::string& string) { set(string); }

#if defined(EA_PLATFORM_WINDOWS)
		// Windows Constructors impl
		path(const wchar_t* wstring) { set(wstring); }
		path(const std::wstring& wstring) { set(wstring); }

		path& operator=(const std::wstring& str) {
			set(str);
			return *this;
		}
#endif

		// Internal set-from string functions
		void set(const std::string& str, path_type type = path_type::native_path);
#if defined(EA_PLATFORM_WINDOWS)
		void set(const std::wstring& wstring, path_type type = native_path);
#endif

		std::string str(path_type type = path_type::native_path) const;
#if defined(EA_PLATFORM_WINDOWS)
		std::wstring wstr(path_type type = native_path) const;
#endif

		bool empty() const { return m_path.empty(); }
		size_t length() const { return m_path.size(); }
		std::string filename() const;
		std::string extension() const;

		size_t file_size() const;
		bool file_exists() const;

		bool is_absolute() const { return m_absolute; }
		bool is_directory() const;
		bool is_file() const;

		path make_absolute() const;
		path parent_path() const;

		// Comparison Operators
		bool operator==(const path& p) const { return p.m_path == m_path; }
		bool operator!=(const path& p) const { return p.m_path != m_path; }

		// Modification Operators
		path operator/(const path& other) const;

	  protected:
		static std::vector<std::string> tokenize(const std::string& string, const std::string& delim);
#if defined(EA_PLATFORM_WINDOWS)
		static const size_t MAX_PATH_WINDOWS = 32767;
#endif
		static const size_t MAX_PATH_WINDOWS_LEGACY = 260;
		std::vector<std::string> m_path;
		path_type m_type;
		bool m_absolute;
	};

	// Utility
	LIBFS_EXPORT path cwd();

	LIBFS_EXPORT bool create_directory(const path& p);
	LIBFS_EXPORT bool create_directories(const path& p);
	LIBFS_EXPORT bool remove_file(const path& p);
	LIBFS_EXPORT bool resize_file(const path& p, size_t target_length);

	// Printing
	LIBFS_EXPORT std::ostream& operator<<(std::ostream& os, const path& path);

} // namespace fs
