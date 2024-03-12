cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(voidjs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_library(voidjs_obj OBJECT
  voidjs/parser/token.cpp
  voidjs/parser/lexer.cpp
  voidjs/parser/parser.cpp
)
target_include_directories(voidjs_obj PUBLIC .)

add_executable(voidjs voidjs.cpp)
target_link_libraries(voidjs PRIVATE voidjs_obj)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  unit_test
  test/test_lexer.cpp
)
target_link_libraries(
  unit_test
  PRIVATE gtest_main
  PRIVATE voidjs_obj
)
include(GoogleTest)
gtest_discover_tests(unit_test)
