set(TARGET_NAME tutorial)
add_executable(
	${TARGET_NAME}
	main.cpp tutorial0.cpp tutorial1.cpp tutorial2.cpp tutorial3.cpp tutorial4.cpp tutorial5.cpp tutorial6.cpp tutorial7.cpp tutorial8.cpp tutorial9.cpp tutorial10.cpp tutorial11_1.cpp tutorial11_2.cpp column1.cpp column2.cpp examplex.cpp
)
include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/engine)
target_link_libraries(${TARGET_NAME} ttengine)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/examples/asset $<TARGET_FILE_DIR:${TARGET_NAME}>/asset)
