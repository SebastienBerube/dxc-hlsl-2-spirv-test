rmdir .\CMakeFiles /s /q
rmdir .\Debug /s /q
rmdir .\x64 /s /q
rmdir .\dxc_test_main.dir /s /q
rmdir .\.vs /s /q
del ALL_BUILD.*
del CMakeCache.txt
del dxc_test_main.*
del ZERO_CHECK.vcxproj*
del cmake_install.cmake
del dxc-test.sln
pause
cmake .
pause