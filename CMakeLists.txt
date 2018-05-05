cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(
    ParticleEffects
        src/ParticleEffects.cpp
        src/Texture.cpp
        src/EnvironmentMap.cpp
        src/Scene.cpp
        src/ParticleEmitter.cpp
        src/BasicShapes.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_impl_glfw_gl3.cpp
        src/glad.c
)
target_link_libraries(ParticleEffects glfw ${OPENGL_gl_LIBRARY} assimp)