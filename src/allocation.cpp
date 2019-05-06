#include "fs/allocation.hpp"
#include <mutex>

namespace fs {
	class mallocator final : public eastl::polyalloc::allocator {
	  public:
		mallocator() = default;
		mallocator(mallocator const&) = delete;
		mallocator(mallocator&&) = delete;
		mallocator& operator=(mallocator const&) = delete;
		mallocator& operator=(mallocator&&) = delete;

		~mallocator() override = default;

		void* allocate(size_t n, int = 0) override { return malloc(n); }
		void* allocate(size_t n, size_t alignment, size_t, int = 0) override { return aligned_alloc(n, alignment); }
		void deallocate(void* p, size_t) override { free(p); }
	};

	static std::mutex global_alloc_guard;
	static mallocator global_alloc_mallocator;
	static eastl::polyalloc::allocator_handle global_alloc_handle = &global_alloc_mallocator;

	void set_global_allocator(eastl::polyalloc::allocator_handle const& handle) {
		std::lock_guard lg(global_alloc_guard);
		global_alloc_handle = handle;
	}

	eastl::polyalloc::allocator_handle get_global_allocator() {
		std::lock_guard lg(global_alloc_guard);
		return global_alloc_handle;
	}

} // namespace fs