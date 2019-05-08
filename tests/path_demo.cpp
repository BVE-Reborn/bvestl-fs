#define DOCTEST_CONFIG_IMPLEMENT

#include "fs/path.hpp"
#include "fs/resolver.hpp"
#include <fmt/format.h>
#include <doctest/doctest.h>
#include <iostream>

using namespace std;
using namespace fs;

std::ostream& operator<<(std::ostream& os, fs::internal::string str) {
	os.write(str.c_str(), str.size());
	return os;
}

fs::internal::string* root;

int main(int argc, char ** argv) {
    doctest::Context context(argc, argv);

    fs::internal::vector<fs::internal::string> args(fs::get_global_allocator());
    for (std::size_t i = 0; i < argc; ++i) {
        args.emplace_back(argv[i], fs::get_global_allocator());
    }

    fs::internal::string root_item(fs::get_global_allocator());

    if (args.size() >= 3) {
        std::size_t end1 = args.size() - 2;
        std::size_t end2 = args.size() - 1;

        if (args[end1] != fs::internal::string("--", fs::get_global_allocator())) {
            fmt::print(stdout, "Need path argument after --", "");
            return 1;
        }

        root_item = args[end2];
    }
    else {
        fmt::print(stdout, "Not enough arguments.");
        return 1;
    }

    root = &root_item;

    fs::create_directory_recursive(fs::path(*root));

    int const res = context.run(); // run

    fs::remove_directory_recursive(fs::path(*root));

    if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;           // propagate the result of the tests

    return res; // the result from doctest is propagated here as well
}

//int main(int argc, char** argv) {
//#if !defined(EA_PLATFORM_WINDOWS)
//	path path1("/dir 1/dir 2/");
//#else
//	path path1("C:\\dir 1\\dir 2\\");
//#endif
//	path path2("dir 3");
//	path path3(path1); // NOLINT(performance-unnecessary-copy-initialization)
//
//	cout << path1.file_exists() << endl;
//	cout << path1 << endl;
//	cout << (path1 / path2) << endl;
//	cout << (path1 / path2).parent_path() << endl;
//	cout << (path1 / path2).parent_path().parent_path() << endl;
//	cout << (path1 / path2).parent_path().parent_path().parent_path() << endl;
//	cout << (path1 / path2).parent_path().parent_path().parent_path().parent_path() << endl;
//	cout << path().parent_path() << endl;
//	cout << "some/path.ext:operator==() = " << (path("some/path.ext") == path("some/path.ext")) << endl;
//	cout << "some/path.ext:operator==() (unequal) = " << (path("some/path.ext") == path("another/path.ext")) << endl;
//
//	cout << "nonexistant:file_exists = " << path("nonexistant").file_exists() << endl;
//	cout << "nonexistant:is_file = " << path("nonexistant").is_file() << endl;
//	cout << "nonexistant:is_directory = " << path("nonexistant").is_directory() << endl;
//	cout << "nonexistant:filename = " << path("nonexistant").filename() << endl;
//	cout << "nonexistant:extension = " << path("nonexistant").extension() << endl;
//	//	cout << "include/fs/path.hpp:exists = " << path("include/fs/path.hpp").file_exists() << endl;
//	//	cout << "include/fs/path.hpp:is_file = " << path("include/fs/path.hpp").is_file() << endl;
//	//	cout << "include/fs/path.hpp:is_directory = " << path("include/fs/path.hpp").is_directory() << endl;
//	//	cout << "include/fs/path.hpp:filename = " << path("include/fs/path.hpp").filename() << endl;
//	//	cout << "include/fs/path.hpp:extension = " << path("include/fs/path.hpp").extension() << endl;
//	//	cout << "include/fs/path.hpp:make_absolute = " << path("include/fs/path.hpp").make_absolute() << endl;
//	cout << "../include/fs:file_exists = " << path("../include/fs").file_exists() << endl;
//	cout << "../include/fs:is_file = " << path("../include/fs").is_file() << endl;
//	cout << "../include/fs:is_directory = " << path("../include/fs").is_directory() << endl;
//	cout << "../include/fs:extension = " << path("../include/fs").extension() << endl;
//	cout << "../include/fs:filename = " << path("../include/fs").filename() << endl;
//	cout << "../include/fs:make_absolute = " << path("../include/fs").make_absolute() << endl;
//
//	cout << "resolve(include/fs/path.hpp) = " << resolver().resolve(fs::path("include/fs/path.hpp")) << endl;
//	cout << "resolve(nonexistant) = " << resolver().resolve(fs::path("nonexistant")) << endl;
//	cout << "copy 1 = " << path3 << '\n';
//	return 0;
//}
