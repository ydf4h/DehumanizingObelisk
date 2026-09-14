cmake_minimum_required(VERSION 3.28)
project(renderer.elf)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} renderer.cpp glad.c)

add_library(OBJ_parser STATIC OBJ_parser.cpp)
add_library(shader STATIC shader.cpp)
add_library(model STATIC model.cpp)
add_library(DeOui STATIC DeOui.cpp)
add_library(event STATIC eventHandler.cpp)
add_library(BMP_parser STATIC BMP_parser.cpp)

find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} OBJ_parser shader model DeOui event BMP_parser glfw)
target_link_libraries(OBJ_parser shader model)
target_link_libraries(model OBJ_parser)

target_include_directories(${PROJECT_NAME} PRIVATE ../include)
target_include_directories(OBJ_parser PRIVATE ../include)
target_include_directories(shader PRIVATE ../include)
target_include_directories(model PRIVATE ../include)
target_include_directories(DeOui PRIVATE ../include)
target_include_directories(event PRIVATE ../include)
target_include_directories(BMP_parser PRIVATE ../include)
