set(SOURCE model.cpp mesh.cpp tex_manager.cpp frame_buffer.cpp shader.cpp attr_array.cpp)
add_library(model ${SOURCE})

find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(FreeGLUT CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_include_directories(model PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(model PUBLIC glm OpenGL::GL GLEW::GLEW FreeGLUT::freeglut assimp::assimp)
