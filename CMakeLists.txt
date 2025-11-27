cmake_minimum_required(VERSION 3.25)
project(graphics)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(glm)
add_subdirectory(src)
add_subdirectory(app)

file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
