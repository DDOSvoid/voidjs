cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(voidjs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_library(voidjs_obj OBJECT voidjs/parser/token.cpp)
target_include_directories(voidjs_obj PUBLIC .)

add_executable(voidjs voidjs.cpp)
target_link_libraries(voidjs PRIVATE voidjs_obj)


