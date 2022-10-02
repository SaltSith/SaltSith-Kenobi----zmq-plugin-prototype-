cmake_minimum_required(VERSION 3.0)

project(kenobi_app
        VERSION 1.0.2
        LANGUAGES C)


add_executable(${CMAKE_PROJECT_NAME} main.c)

add_subdirectory(plugin)
target_link_libraries(${CMAKE_PROJECT_NAME} plugin)

add_subdirectory(plugin/plugin_socket)
target_link_libraries(${CMAKE_PROJECT_NAME} plugin_socket)

target_link_libraries(${CMAKE_PROJECT_NAME} rt)
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)

target_link_libraries(${CMAKE_PROJECT_NAME} czmq)
target_link_libraries(${CMAKE_PROJECT_NAME} zmq)

# testing 
enable_testing()

#testing plugin_socket
add_executable(test_plugin_socket plugin/plugin_socket/test/test.c)
target_link_libraries(test_plugin_socket plugin_socket)
target_link_libraries(test_plugin_socket czmq)
target_link_libraries(test_plugin_socket zmq)
add_test(NAME test_plugin_socket COMMAND test_plugin_socket)

#testing plugin_task
add_executable(test_plugin_task plugin/test/test.c)
target_link_libraries(test_plugin_task plugin)
target_link_libraries(test_plugin_task plugin_socket)
target_link_libraries(test_plugin_task czmq)
target_link_libraries(test_plugin_task zmq)
add_test(NAME test_plugin_task COMMAND test_plugin_task)
