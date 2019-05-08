/*
    resolver.h -- A simple class for cross-platform path resolution

    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "fs/allocation.hpp"
#include "fs/path.hpp"
#include <EASTL/vector.h>

namespace fs {

	/**
	 * \brief Simple class for resolving paths on Linux/Windows/Mac OS
	 *
	 * This convenience class looks for a file or directory given its name
	 * and a set of search paths. The implementation walks through the
	 * search paths in order and stops once the file is found.
	 */
	class LIBFS_EXPORT resolver {
	  public:
		using iterator = internal::vector<path>::iterator;
		using const_iterator = internal::vector<path>::const_iterator;

		explicit resolver(eastl::polyalloc::allocator_handle const handle LIBFS_GET_GLOBAL_ALLOC) : m_paths(handle) {
			m_paths.push_back(cwd());
		}

		size_t size() const { return m_paths.size(); }

		iterator begin() { return m_paths.begin(); }
		iterator end() { return m_paths.end(); }

		const_iterator begin() const { return m_paths.begin(); }
		const_iterator end() const { return m_paths.end(); }

		void erase(iterator const it) { m_paths.erase(it); }

		void prepend(const path& path) { m_paths.insert(m_paths.begin(), path); }
		void append(const path& path) { m_paths.push_back(path); }
		const path& operator[](size_t const index) const { return m_paths[index]; }
		path& operator[](size_t const index) { return m_paths[index]; }

		path resolve(const path& value) const;

		friend std::ostream& operator<<(std::ostream&, const resolver&);

	  private:
		internal::vector<path> m_paths;
	};

	LIBFS_EXPORT std::ostream& operator<<(std::ostream& os, const resolver& r);

} // namespace fs
