@echo off
set dxc_git_tree=dc717dfebb4ff7534e23417536c0198ad3104f57
set VCPKG_REGISTRY=D:\Dev\Perso\GitHub\vcpkg-registry
echo This should open the file in a text editor
%APPDATA%\..\Local\vcpkg\registries\git-trees\dc717dfebb4ff7534e23417536c0198ad3104f57\portfile.cmake
pause
echo Copying local portfile.cmake to vcpkg-registry repository
copy %APPDATA%\..\Local\vcpkg\registries\git-trees\dc717dfebb4ff7534e23417536c0198ad3104f57\portfile.cmake %VCPKG_REGISTRY%\ports\dxc\portfile.cmake
echo Cleaning previous vcpkg installed, buildtrees and packages files
rmdir %VCPKG_PATH%\installed /s /q
rmdir %VCPKG_PATH%\packages /s /q
rmdir %VCPKG_PATH%\buildtrees /s /q
echo Cleaning app data vcpkg archive
rmdir %APPDATA%\..\Local\vcpkg\archives /s /q
echo Cleaning current project vcpkg packages
rmdir .\vcpkg_installed /s /q
pause
echo Installing vcpkg with updated portfile
vcpkg install --feature-flags=registries,manifests --triplet x64-windows
pause
