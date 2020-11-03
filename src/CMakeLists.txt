include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/glm")

set(SOURCE model.cpp mesh.cpp tex_manager.cpp frame_buffer.cpp)
add_library(model ${SOURCE})
target_link_libraries(model opengl32 glew32 freeglut glu32 glm assimp)
