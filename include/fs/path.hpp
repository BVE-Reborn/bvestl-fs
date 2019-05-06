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
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>
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
	class FS_EXPORT path {
	  public:
		enum path_type {
			windows_path = 0,
			posix_path = 1,
#if defined(EA_PLATFORM_WINDOWS)
			native_path = windows_path
#else
			native_path = posix_path
#endif
		};

		path() : m_type(native_path), m_absolute(false) {}

		path(const path& path) = default;
		path(path&& path) = default;
		path& operator=(const path& path) = default;
		path& operator=(path&& path) = default;

		explicit path(const char* string) {
			set(string);
		}

		explicit path(const std::string& string) {
			set(string);
		}

#if defined(EA_PLATFORM_WINDOWS)
		path(const std::wstring& wstring) {
			set(wstring);
		}
		path(const wchar_t* wstring) {
			set(wstring);
		}

		path& operator=(const std::wstring& str) {
			set(str);
			return *this;
		}
#endif

		size_t length() const {
			return m_path.size();
		}

		bool empty() const {
			return m_path.empty();
		}

		bool is_absolute() const {
			return m_absolute;
		}

		path make_absolute() const;

		bool exists() const;

		size_t file_size() const;

		bool is_directory() const;

		bool is_file() const;

		std::string extension() const;

		std::string filename() const;

		path parent_path() const;

		path operator/(const path& other) const;

		std::string str(path_type type = native_path) const;

		void set(const std::string& str, path_type type = native_path);

#if defined(EA_PLATFORM_WINDOWS)
		std::wstring wstr(path_type type = native_path) const;

		void set(const std::wstring& wstring, path_type type = native_path);
#endif

		FS_EXPORT friend std::ostream& operator<<(std::ostream& os, const path& path);

		bool remove_file();

		bool resize_file(size_t target_length);

		static path getcwd();

		bool operator==(const path& p) const {
			return p.m_path == m_path;
		}
		bool operator!=(const path& p) const {
			return p.m_path != m_path;
		}

	  protected:
		static std::vector<std::string> tokenize(const std::string& string, const std::string& delim);

	  protected:
#if defined(EA_PLATFORM_WINDOWS)
		static const size_t MAX_PATH_WINDOWS = 32767;
#endif
		static const size_t MAX_PATH_WINDOWS_LEGACY = 260;
		path_type m_type;
		std::vector<std::string> m_path;
		bool m_absolute;
	};

	FS_EXPORT inline bool create_directory(const path& p);

	FS_EXPORT inline bool create_directories(const path& p);

} // namespace fs
