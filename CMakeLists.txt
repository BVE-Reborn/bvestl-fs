cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(fs LANGUAGES CXX)

include(GenerateExportHeader)
include(GNUInstallDirs)

find_package(eastl-polyalloc CONFIG REQUIRED)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE HEADERS LIST_DIRECTORIES false CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS "tests/*.cpp")

#########
# libfs #
#########

add_library(fs ${SOURCES} ${HEADERS})
target_include_directories(fs PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
target_link_libraries(fs PUBLIC eastl::polyalloc)

generate_export_header(
        fs
        BASE_NAME FS
        EXPORT_FILE_NAME include/fs/api.hpp
)
target_include_directories(fs PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>)

install(
        TARGETS fs
        EXPORT fs-config
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/
)

install(
        EXPORT fs-config DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/fs
        NAMESPACE fs::
)

install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_BINARY_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
        FILES_MATCHING PATTERN "*.hpp*"
)

#########
# tests #
#########
add_executable(path_demo ${TEST_SOURCES})
target_link_libraries(path_demo PRIVATE fs)
