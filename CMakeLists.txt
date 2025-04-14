cmake_minimum_required(VERSION 3.10)

project(MyProject LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")

file(GLOB_RECURSE HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")

add_executable(MyProject
    src/Camera.cpp
    src/CollideCoarse.cpp
    src/Collider.cpp
    src/Contact.cpp 
    src/EBO.cpp
    src/glad.c
    src/Light.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/Player.cpp
    src/RigidBody.cpp
    src/shaderClass.cpp
    src/stb.cpp
    src/Texture.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/World.cpp
    src/UBO.cpp

    z_demos/plinko.cpp
)

target_include_directories(MyProject PRIVATE ${INCLUDE_DIR})

target_link_libraries(MyProject PRIVATE ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

message(STATUS "Detected source files: ${SOURCES}")
message(STATUS "Detected header files: ${HEADERS}")