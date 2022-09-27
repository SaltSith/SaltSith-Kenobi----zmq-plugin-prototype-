cmake_minimum_required(VERSION 3.0)

project(kenobi_app
        VERSION 1.0.2
        LANGUAGES C)


add_executable(${CMAKE_PROJECT_NAME} main.c)

add_subdirectory(plugin)
target_link_libraries(${CMAKE_PROJECT_NAME} plugin)

target_link_libraries(${CMAKE_PROJECT_NAME} rt)
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)

target_link_libraries(${CMAKE_PROJECT_NAME} czmq)
target_link_libraries(${CMAKE_PROJECT_NAME} zmq)
