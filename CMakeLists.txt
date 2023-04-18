cmake_minimum_required(VERSION 3.21)
project(dxc-test)

add_executable(dxc_test_main "main.cpp")
target_compile_features(dxc_test_main PRIVATE cxx_std_14)

IF(UNIX AND NOT APPLE)
	set(LINUX TRUE)
ENDIF()

IF(WIN32)
	message("Using Win32 library")
	find_library(dxc_lib_path NAMES dxcompiler PATHS ${CMAKE_SOURCE_DIR}/libs/dxc/x64)
ELSEIF(LINUX)
	message("Using Linux library")
	find_library(dxc_lib_path NAMES libdxcompiler PATHS ${CMAKE_SOURCE_DIR}/libs/dxc/linux)
ENDIF()

message("find_library result : ${dxc_lib_path}")

target_include_directories(dxc_test_main PRIVATE ${CMAKE_SOURCE_DIR}/external/dxc)

target_link_libraries(dxc_test_main ${dxc_lib_path})