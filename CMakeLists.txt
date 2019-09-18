cmake_minimum_required(VERSION 3.13)
project(Subor)
set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE SOURCE Sources/*.?pp)
include_directories(Sources)
message("${SOURCE}")
add_executable(Subor ${SOURCE})
find_package (SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries (Subor ${SDL2_LIBRARIES})


