cmake_minimum_required(VERSION 3.1)
project(graphics)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(glm)

include_directories(app "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/glm")

set(SOURCE main.cpp)
add_executable(app ${SOURCE})
target_link_libraries(app opengl32 glew32 freeglut glu32 glm)
