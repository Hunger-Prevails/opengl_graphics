cmake_minimum_required(VERSION 3.1)
project(graphics)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(glm)
add_subdirectory(src)

include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/glm")

set(SOURCE_BASICS basics.cpp)
add_executable(basics ${SOURCE_BASICS})
target_link_libraries(basics opengl32 glew32 freeglut glu32 glm)

set(SOURCE_LIGHTS lights.cpp)
add_executable(lights ${SOURCE_LIGHTS})
target_link_libraries(lights opengl32 glew32 freeglut glu32 glm model)

set(SOURCE_MAIN main.cpp)
add_executable(main ${SOURCE_MAIN})
target_link_libraries(main opengl32 glew32 freeglut glu32 glm model)