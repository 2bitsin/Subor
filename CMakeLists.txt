cmake_minimum_required(VERSION 3.13)
project(Subor)
set(CMAKE_CXX_STANDARD 17)

get_filename_component(DEBUG_DATA_PATH "Data" ABSOLUTE)
configure_file(Config.hpp.in Config.hpp @ONLY)
file(GLOB_RECURSE SOURCE Sources/*.?pp)
include_directories(Sources)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
message("cur dir : ${CMAKE_CURRENT_BINARY_DIR}")
add_executable(Subor ${SOURCE})
find_package (SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries (Subor ${SDL2_LIBRARIES})


