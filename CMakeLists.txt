cmake_minimum_required(VERSION 3.10)
project(ttengine VERSION 0.00 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
find_package(Boost REQUIRED)
message("### Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message("### Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
find_package(SDL2 CONFIG REQUIRED)
message("### SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message("### SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
find_package(sdl2-image CONFIG REQUIRED)
add_subdirectory(engine)
add_subdirectory(examples)
