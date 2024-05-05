cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(voidjs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_library(voidjs_obj OBJECT
  voidjs/lexer/token.cpp
  voidjs/lexer/lexer.cpp
  voidjs/ir/ast.cpp
  voidjs/parser/parser.cpp
  voidjs/types/js_value.cpp
  voidjs/types/object_class_type.cpp
  voidjs/types/object_factory.cpp
  voidjs/types/lang_types/object.cpp
  voidjs/types/spec_types/environment_record.cpp
  voidjs/types/spec_types/lexical_environment.cpp
  voidjs/types/spec_types/property_descriptor.cpp
  voidjs/types/internal_types/array.cpp
  voidjs/builtins/builtin.cpp
  voidjs/builtins/js_object.cpp
  voidjs/builtins/js_function.cpp
  voidjs/builtins/js_array.cpp
  voidjs/builtins/js_string.cpp
  voidjs/builtins/js_boolean.cpp
  voidjs/builtins/js_number.cpp
  voidjs/interpreter/interpreter.cpp
  voidjs/interpreter/execution_context.cpp
)
target_include_directories(voidjs_obj PUBLIC .)

add_executable(voidjs voidjs/voidjs.cpp)
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
  test/test_parser.cpp
  test/test_interpreter.cpp
  test/test_js_value.cpp
  test/test_internal_types.cpp
  test/test_builtins.cpp
)
target_link_libraries(
  unit_test
  PRIVATE gtest_main
  PRIVATE voidjs_obj
)
include(GoogleTest)
gtest_discover_tests(unit_test)
