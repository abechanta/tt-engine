add_library(ttengine STATIC tte_main.cpp)
include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} .)
target_link_libraries(ttengine PRIVATE ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

#find_package(SDL2 CONFIG REQUIRED)
#target_link_libraries(ttengine PRIVATE SDL2::SDL2 SDL2::SDL2main)
