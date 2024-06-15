add_executable(Subor 
  core/Console.cpp
  fileio/ProgramROM.cpp
  frontend/Main.cpp
  mapper/Mapper.cpp
  mapper/MapperMMC1.cpp
  mapper/MapperNROM.cpp
  mapper/MapperUxROM.cpp
  test/Nestest.cpp
  test/NestestLog.cpp
  test/NestestMain.cpp
  test/NestestROM.cpp
)
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(Subor PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_compile_features(Subor PRIVATE cxx_std_23)
target_include_directories(Subor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})