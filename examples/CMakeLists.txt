set(TARGET_NAME tutorial)
add_executable(
	${TARGET_NAME}
	main.cpp tutorial1.cpp tutorial2.cpp tutorial3.cpp tutorial4.cpp tutorial5.cpp tutorial6.cpp tutorial7.cpp tutorial8.cpp tutorial9.cpp tutorial10.cpp tutorial11.cpp tutorial12.cpp tutorial13.cpp tutorial14.cpp column1.cpp column2.cpp sdl_splash.cpp sdl_app.cpp
)
include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/engine)
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_17)
target_link_libraries(${TARGET_NAME} ttengine)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/examples/asset $<TARGET_FILE_DIR:${TARGET_NAME}>/asset)
