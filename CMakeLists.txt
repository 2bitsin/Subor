cmake_minimum_required(VERSION 3.13)
project(Subor)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /AWAIT")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000000")

if(NOT DEFINED ${DEBUG_DATA_PATH})
	get_filename_component(DEBUG_DATA_PATH "Data" ABSOLUTE)
endif(NOT DEFINED ${DEBUG_DATA_PATH})

configure_file(Config.hpp.in Config.hpp @ONLY)
file(GLOB_RECURSE SOURCE Sources/*.?pp)
include_directories(Sources)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(Subor ${SOURCE})

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(Subor PRIVATE SDL2::SDL2)



message("cur dir : ${CMAKE_CURRENT_BINARY_DIR}")

