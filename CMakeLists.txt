cmake_minimum_required(VERSION 3.10)
project(ttengine VERSION 0.00 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
find_package(Boost 1.67 REQUIRED)
message("### Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message("### Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
find_package(SDL2 REQUIRED)
message("### SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message("### SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
find_package(SDL2_image REQUIRED)
message("### SDL2_IMAGE_INCLUDE_DIRS: ${SDL2_IMAGE_INCLUDE_DIRS}")
message("### SDL2_IMAGE_LIBRARIES: ${SDL2_IMAGE_LIBRARIES}")
add_subdirectory(engine)
add_subdirectory(examples)
