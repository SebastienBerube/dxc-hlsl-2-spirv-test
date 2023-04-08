set(CMAKE_TOOLCHAIN_FILE "C:/Dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

cmake_minimum_required(VERSION 3.21)
project(dxc-test)

add_executable(dxc_test_main "main.cpp")
target_compile_features(dxc_test_main PRIVATE cxx_std_14)

find_package(dxc CONFIG REQUIRED)
target_link_libraries(dxc_test_main dxc::dxc)