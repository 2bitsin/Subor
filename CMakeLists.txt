cmake_minimum_required(VERSION 3.24)
set (CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set (VCPKG_TARGET_TRIPLET "x64-windows-static")
project(Subor)

add_subdirectory(Sources)
file(GLOB _RomAssets "Data/*.nes")
install(FILES ${_RomAssets} DESTINATION ${CMAKE_INSTALL_PREFIX}/dat)
install(TARGETS Subor DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)