cmake_minimum_required(VERSION 3.21)
project(dxc-test)

add_executable(dxc_test_main "main.cpp")
target_compile_features(dxc_test_main PRIVATE cxx_std_14)

# find_package(dxc CONFIG REQUIRED)
# find_library()
find_library(dxc NAMES dxcompiler libdxcompiler dxcompiler vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/dxc)

target_link_libraries(dxc_test_main dxc::dxc)