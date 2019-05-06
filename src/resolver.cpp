#include "fs/resolver.hpp"

fs::path fs::resolver::resolve(fs::path const& value) const {
	for (const auto& m_path : m_paths) {
		path combined = m_path / value;
		if (combined.exists())
			return combined;
	}
	return value;
}

std::ostream& fs::operator<<(std::ostream& os, fs::resolver const& r) {
	os << "resolver[" << std::endl;
	for (size_t i = 0; i < r.m_paths.size(); ++i) {
		os << "  \"" << r.m_paths[i] << "\"";
		if (i + 1 < r.m_paths.size())
			os << ",";
		os << std::endl;
	}
	os << "]";
	return os;
}
