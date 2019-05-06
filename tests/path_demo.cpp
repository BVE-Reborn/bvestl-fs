#include "fs/path.hpp"
#include "fs/resolver.hpp"
#include <iostream>

using namespace std;
using namespace fs;

std::ostream& operator<<(std::ostream& os, fs::internal::string str) {
	os.write(str.c_str(), str.size());
	return os;
}

int main(int argc, char** argv) {
#if !defined(EA_PLATFORM_WINDOWS)
	path path1("/dir 1/dir 2/");
#else
	path path1("C:\\dir 1\\dir 2\\");
#endif
	path path2("dir 3");
	path path3(path1); // NOLINT(performance-unnecessary-copy-initialization)

	cout << path1.file_exists() << endl;
	cout << path1 << endl;
	cout << (path1 / path2) << endl;
	cout << (path1 / path2).parent_path() << endl;
	cout << (path1 / path2).parent_path().parent_path() << endl;
	cout << (path1 / path2).parent_path().parent_path().parent_path() << endl;
	cout << (path1 / path2).parent_path().parent_path().parent_path().parent_path() << endl;
	cout << path().parent_path() << endl;
	cout << "some/path.ext:operator==() = " << (path("some/path.ext") == path("some/path.ext")) << endl;
	cout << "some/path.ext:operator==() (unequal) = " << (path("some/path.ext") == path("another/path.ext")) << endl;

	cout << "nonexistant:file_exists = " << path("nonexistant").file_exists() << endl;
	cout << "nonexistant:is_file = " << path("nonexistant").is_file() << endl;
	cout << "nonexistant:is_directory = " << path("nonexistant").is_directory() << endl;
	cout << "nonexistant:filename = " << path("nonexistant").filename() << endl;
	cout << "nonexistant:extension = " << path("nonexistant").extension() << endl;
	//	cout << "include/fs/path.hpp:exists = " << path("include/fs/path.hpp").file_exists() << endl;
	//	cout << "include/fs/path.hpp:is_file = " << path("include/fs/path.hpp").is_file() << endl;
	//	cout << "include/fs/path.hpp:is_directory = " << path("include/fs/path.hpp").is_directory() << endl;
	//	cout << "include/fs/path.hpp:filename = " << path("include/fs/path.hpp").filename() << endl;
	//	cout << "include/fs/path.hpp:extension = " << path("include/fs/path.hpp").extension() << endl;
	//	cout << "include/fs/path.hpp:make_absolute = " << path("include/fs/path.hpp").make_absolute() << endl;
	cout << "../include/fs:file_exists = " << path("../include/fs").file_exists() << endl;
	cout << "../include/fs:is_file = " << path("../include/fs").is_file() << endl;
	cout << "../include/fs:is_directory = " << path("../include/fs").is_directory() << endl;
	cout << "../include/fs:extension = " << path("../include/fs").extension() << endl;
	cout << "../include/fs:filename = " << path("../include/fs").filename() << endl;
	cout << "../include/fs:make_absolute = " << path("../include/fs").make_absolute() << endl;

	cout << "resolve(include/fs/path.hpp) = " << resolver().resolve(fs::path("include/fs/path.hpp")) << endl;
	cout << "resolve(nonexistant) = " << resolver().resolve(fs::path("nonexistant")) << endl;
	cout << "copy 1 = " << path3 << '\n';
	return 0;
}
