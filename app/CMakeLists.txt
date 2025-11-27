set(SOURCE_LIGHTS lights.cpp)
add_executable(lights ${SOURCE_LIGHTS})

target_link_libraries(lights model)

set(SOURCE_MAIN main.cpp)
add_executable(main ${SOURCE_MAIN})

target_link_libraries(main model)
