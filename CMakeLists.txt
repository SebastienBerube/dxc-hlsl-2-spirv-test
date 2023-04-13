cmake_minimum_required(VERSION 3.21)
project(dxc-test)

add_executable(dxc_test_main "main.cpp")
target_compile_features(dxc_test_main PRIVATE cxx_std_14)

message("Before find_library")
# find_package(dxc CONFIG REQUIRED)
# find_library()

find_library(dxc NAMES dxcompiler libdxcompiler dxcompiler vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/dxc)
message("Before target_link_libraries")

# Add include directories to a target.
# Specifies include directories to use when compiling a given target. The named <target> must have been created
# target_include_directories(<target> [SYSTEM] [AFTER|BEFORE]
#                            <INTERFACE|PUBLIC|PRIVATE> [items1...]
#                            [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])
target_include_directories(dxc_test_main PRIVATE ${CMAKE_SOURCE_DIR}/external/dxc)

target_link_libraries(dxc_test_main dxcompiler)