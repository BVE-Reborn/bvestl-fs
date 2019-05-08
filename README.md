# libfs

A port of [wjakob's filesystem/path.h](https://github.com/wjakob/filesystem) to EASTL, including complete support for custom allocators. Added various functionality to round out the capibilities of the library. API can be found in include/path.hpp.

## Installation/Building

Uses vcpkg as primary package manager, though is usable without. Requires:

- `doctest`
- `eastl`
- `eastl-polyalloc` (vcpkg port found in external/vcpkg/ports/)
- `fmt`
