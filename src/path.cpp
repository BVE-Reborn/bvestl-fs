#include "fs/path.hpp"

#if defined(EA_PLATFORM_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <ShlObj.h>
#	include <Windows.h>
#else
#	include <unistd.h>
#	include <sys/stat.h>
#endif

#if defined(EA_PLATFORM_LINUX)
#	include <linux/limits.h>
#endif

#include <cstring>
#include <sstream>
#include <stdexcept>

fs::path fs::path::make_absolute() const {
#if !defined(_WIN32)
	char temp[PATH_MAX];
	if (realpath(str().c_str(), temp) == NULL)
		throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
	return path(temp);
#else
	std::wstring value = wstr(), out(MAX_PATH_WINDOWS, '\0');
	DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH_WINDOWS, &out[0], NULL);
	if (length == 0)
		throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
	return path(out.substr(0, length));
#endif
}

bool fs::path::file_exists() const {
#if defined(_WIN32)
	return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
	struct stat sb {};
	return stat(str().c_str(), &sb) == 0;
#endif
}

size_t fs::path::file_size() const {
#if defined(_WIN32)
	struct _stati64 sb;
	if (_wstati64(wstr().c_str(), &sb) != 0)
		throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#else
	struct stat sb {};
	if (stat(str().c_str(), &sb) != 0)
		throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#endif
	return (size_t) sb.st_size;
}

bool fs::path::is_directory() const {
#if defined(_WIN32)
	DWORD result = GetFileAttributesW(wstr().c_str());
	if (result == INVALID_FILE_ATTRIBUTES)
		return false;
	return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
	struct stat sb {};
	if (stat(str().c_str(), &sb))
		return false;
	return S_ISDIR(sb.st_mode);
#endif
}

bool fs::path::is_file() const {
#if defined(_WIN32)
	DWORD attr = GetFileAttributesW(wstr().c_str());
	return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	struct stat sb {};
	if (stat(str().c_str(), &sb))
		return false;
	return S_ISREG(sb.st_mode);
#endif
}

std::string fs::path::extension() const {
	const std::string& name = filename();
	size_t pos = name.find_last_of('.');
	if (pos == std::string::npos)
		return "";
	return name.substr(pos + 1);
}

std::string fs::path::filename() const {
	if (empty())
		return "";
	const std::string& last = m_path[m_path.size() - 1];
	return last;
}

fs::path fs::path::parent_path() const {
	path result;
	result.m_absolute = m_absolute;

	if (m_path.empty()) {
		if (!m_absolute)
			result.m_path.emplace_back("..");
	}
	else {
		size_t until = m_path.size() - 1;
		for (size_t i = 0; i < until; ++i)
			result.m_path.push_back(m_path[i]);
	}
	return result;
}

fs::path fs::path::operator/(fs::path const& other) const {
	if (other.m_absolute)
		throw std::runtime_error("path::operator/(): expected a relative path!");
	if (m_type != other.m_type)
		throw std::runtime_error("path::operator/(): expected a path of the same type!");

	path result(*this);

	for (const auto& i : other.m_path) {
		result.m_path.push_back(i);
	}

	return result;
}

std::string fs::path::str(fs::path::path_type type) const {
	std::ostringstream oss;

	if (m_absolute) {
		if (m_type == path_type::posix_path)
			oss << "/";
		else {
			size_t length = 0;
			for (const auto& i : m_path) {
				// No special case for the last segment to count the NULL character
				length += i.length() + 1;
			}
			// Windows requires a \\?\ prefix to handle paths longer than MAX_PATH
			// (including their null character). NOTE: relative paths >MAX_PATH are
			// not supported at all in Windows.
			if (length > MAX_PATH_WINDOWS_LEGACY)
				oss << R"(\\?\)";
		}
	}

	for (size_t i = 0; i < m_path.size(); ++i) {
		oss << m_path[i];
		if (i + 1 < m_path.size()) {
			if (type == path_type::posix_path)
				oss << '/';
			else
				oss << '\\';
		}
	}

	return oss.str();
}

void fs::path::set(const std::string& str, fs::path::path_type type) {
	m_type = type;
	if (type == path_type::windows_path) {
		std::string tmp = str;

		// Long windows paths (sometimes) begin with the prefix \\?\. It should only
		// be used when the path is >MAX_PATH characters long, so we remove it
		// for convenience and add it back (if necessary) in str()/wstr().
		static const std::string PREFIX = R"(\\?\)";
		if (tmp.length() >= PREFIX.length()
		    && std::mismatch(std::begin(PREFIX), std::end(PREFIX), std::begin(tmp)).first == std::end(PREFIX)) {
			tmp.erase(0, 4);
		}
		m_path = tokenize(tmp, "/\\");
		m_absolute = tmp.size() >= 2 && std::isalpha(tmp[0]) && tmp[1] == ':';
	}
	else {
		m_path = tokenize(str, "/");
		m_absolute = !str.empty() && str[0] == '/';
	}
}

std::ostream& fs::operator<<(std::ostream& os, fs::path const& path) {
	os << path.str();
	return os;
}

bool fs::create_directory(fs::path const& p) {
#if defined(_WIN32)
	return CreateDirectoryW(p.wstr().c_str(), NULL) != 0;
#else
	return mkdir(p.str().c_str(), S_IRWXU) == 0;
#endif
}

bool fs::create_directories(fs::path const& p) {
#if defined(_WIN32)
	return SHCreateDirectory(nullptr, p.make_absolute().wstr().c_str()) == ERROR_SUCCESS;
#else
	if (create_directory(path(p.str().c_str())))
		return true;

	if (p.empty())
		return false;

	if (errno == ENOENT) {
		if (create_directory(p.parent_path()))
			return mkdir(p.str().c_str(), S_IRWXU) == 0;
		else
			return false;
	}
	return false;
#endif
}

bool fs::remove_file(const path& p) {
#if !defined(_WIN32)
	return std::remove(p.str().c_str()) == 0;
#else
	return DeleteFileW(wstr().c_str()) != 0;
#endif
}

bool fs::resize_file(const path& p, size_t target_length) {
#if !defined(_WIN32)
	return ::truncate(p.str().c_str(), (off_t) target_length) == 0;
#else
	HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (handle == INVALID_HANDLE_VALUE)
		return false;
	LARGE_INTEGER size;
	size.QuadPart = (LONGLONG) target_length;
	if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0) {
		CloseHandle(handle);
		return false;
	}
	if (SetEndOfFile(handle) == 0) {
		CloseHandle(handle);
		return false;
	}
	CloseHandle(handle);
	return true;
#endif
}

fs::path fs::cwd() {
#if !defined(_WIN32)
	char temp[PATH_MAX];
	if (::getcwd(temp, PATH_MAX) == NULL)
		throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
	return path(temp);
#else
	std::wstring temp(MAX_PATH_WINDOWS, '\0');
	if (!_wgetcwd(&temp[0], MAX_PATH_WINDOWS))
		throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
	return path(temp.c_str());
#endif
}

std::vector<std::string> fs::path::tokenize(const std::string& string, const std::string& delim) {
	std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
	std::vector<std::string> tokens;

	while (lastPos != std::string::npos) {
		if (pos != lastPos)
			tokens.push_back(string.substr(lastPos, pos - lastPos));
		lastPos = pos;
		if (lastPos == std::string::npos || lastPos + 1 == string.length())
			break;
		pos = string.find_first_of(delim, ++lastPos);
	}

	return tokens;
}

#if defined(_WIN32)
std::wstring fs::path::wstr(fs::path::path_type type) const {
	std::string temp = str(type);
	int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int) temp.size(), NULL, 0);
	std::wstring result(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int) temp.size(), &result[0], size);
	return result;
}

void fs::path::set(const std::wstring& wstring, fs::path::path_type type) {
	std::string string;
	if (!wstring.empty()) {
		int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int) wstring.size(), NULL, 0, NULL, NULL);
		string.resize(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int) wstring.size(), &string[0], size, NULL, NULL);
	}
	set(string, type);
}
#endif
