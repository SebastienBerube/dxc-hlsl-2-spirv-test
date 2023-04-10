if(DEFINED ENV{VCPKG_PATH})
    message("VCPKG_PATH was found")
else(DEFINED ENV{VCPKG_PATH})
    message( FATAL_ERROR "VCPKG_PATH environment variable not found! Try cloning https://github.com/microsoft/vcpkg and setting VCPKG_PATH environment variable to the root path of vcpkg repository. (e.g. VCPKG_PATH=c:/dev/vcpkg)")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message("VCPKG_PATH=$ENV{VCPKG_PATH}")

cmake_minimum_required(VERSION 3.21)
project(dxc-test)

add_executable(dxc_test_main "main.cpp")
target_compile_features(dxc_test_main PRIVATE cxx_std_14)

find_package(dxc CONFIG REQUIRED)

target_link_libraries(dxc_test_main dxc::dxc)