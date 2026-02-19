cmake_minimum_required(VERSION 3.10)
project(COMP3015)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(comp3015
    main.cpp
    scenebasic_uniform.cpp
    helper/trianglemesh.cpp
    helper/objmesh.cpp
    helper/plane.cpp
    helper/glslprogram.cpp
    helper/glutils.cpp
    helper/glad.c
)

target_include_directories(comp3015 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/helper
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)

target_link_libraries(comp3015 PRIVATE
    OpenGL::OpenGL
    glfw
    glm::glm
)

target_compile_options(comp3015 PRIVATE -Wall -Wextra -Wpedantic)
