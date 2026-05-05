cmake_minimum_required(VERSION 3.10)
project(COMP3015)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(Bullet REQUIRED)

add_executable(comp3015
    main.cpp
    scenebasic_uniform.cpp
    core/mesh_instance.cpp
    core/physics_world.cpp
    core/flycamera.cpp
    core/projectile_system.cpp
    core/particle_system.cpp
    core/skybox.cpp
    core/crosshair.cpp
    core/post_process.cpp
    core/hud.cpp
    helper/assimpmesh.cpp
    helper/scenerunner.cpp
    helper/glslprogram.cpp
    helper/glutils.cpp
    helper/texture.cpp
    helper/skybox.cpp
    helper/trianglemesh.cpp

    helper/stb/stb_image.cpp
    glad/src/glad.c
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(comp3015 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/helper
    ${CMAKE_CURRENT_SOURCE_DIR}/structs
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
    ${BULLET_INCLUDE_DIRS}
)

target_link_libraries(comp3015 PRIVATE
    OpenGL::OpenGL
    glfw
    glm::glm
    assimp
    ${BULLET_LIBRARIES}
)

target_compile_options(comp3015 PRIVATE -Wall -Wextra -Wpedantic)
