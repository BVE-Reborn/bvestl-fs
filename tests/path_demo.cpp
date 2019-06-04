#define DOCTEST_CONFIG_IMPLEMENT

#include "bvestl/fs/path.hpp"
#include "bvestl/fs/resolver.hpp"
#include <doctest/doctest.h>
#include <fmt/format.h>
#include <iostream>

std::ostream& operator<<(std::ostream& os, bvestl::fs::internal::string str) {
	os.write(str.c_str(), str.size());
	return os;
}

bvestl::fs::path* root;

class FileSystemTestFixture {
public:
	FileSystemTestFixture() {
		bvestl::fs::create_directory_recursive(*root);
	}
	~FileSystemTestFixture() {
		bvestl::fs::remove_directory_recursive(*root);
	}
};

TEST_CASE_FIXTURE(FileSystemTestFixture, "List Directory") {
	auto result = bvestl::fs::list_directory_recursive(root->parent_path());

	for(auto& p : result) {
		 std::cout << p.str() << '\n';
	}
}

int main(int const argc, char** argv) {
	doctest::Context context(argc, argv);

	bvestl::fs::internal::vector<bvestl::fs::internal::string> args(bvestl::fs::get_global_allocator());
	for (std::size_t i = 0; i < argc; ++i) {
		args.emplace_back(argv[i], bvestl::fs::get_global_allocator());
	}

	bvestl::fs::internal::string root_item(bvestl::fs::get_global_allocator());

	if (args.size() >= 3) {
		std::size_t const end1 = args.size() - 2;
		std::size_t const end2 = args.size() - 1;

		if (args[end1] != bvestl::fs::internal::string("--", bvestl::fs::get_global_allocator())) {
			fmt::print(stdout, "Need path argument after --", "");
			return 1;
		}

		root_item = args[end2];
	}
	else {
		fmt::print(stdout, "Not enough arguments.");
		return 1;
	}

	bvestl::fs::path root_path(root_item);
	bvestl::fs::create_directory_recursive(root_path);
	root_path = root_path.make_absolute();
	root = &root_path;

	int const res = context.run(); // run

	if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
		return res;           // propagate the result of the tests

	return res; // the result from doctest is propagated here as well
}
