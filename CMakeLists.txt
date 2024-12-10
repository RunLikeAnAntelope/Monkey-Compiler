cmake_minimum_required(VERSION 3.22)
project(Lexer CXX)

# Put output in seperate directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Set C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Export compile_commands.json for nvim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# put cmake stuff in bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# compile flags
set (CMAKE_CXX_FLAGS "-ferror-limit=0 -Wall -Wextra -Wextra -Wconversion -Wsign-conversion -pedantic-errors -g -std=c++20")
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# set sources
set(SOURCES 
    "src/token.hpp"
    "src/token.cpp"
    "src/lexer.hpp"
    "src/lexer.cpp"
    "src/repl.hpp"
    "src/repl.cpp"
    "src/ast.hpp"
    "src/ast.cpp"
    "src/parser.hpp"
    "src/parser.cpp"
    "src/helpers.hpp"
    "src/helpers.cpp"
    "src/object.hpp"
    "src/object.cpp"
    "src/evaluator.hpp"
    "src/evaluator.cpp"
    "src/common.hpp"
)

set(TESTS
    "test/ast_test.cpp"
    "test/tests.cpp"
    "test/parser_test.cpp"
    "test/evaluator_test.cpp"
)


# Link runTests with what we want to test and the GTest and pthread library
add_executable(Test ${SOURCES} ${TESTS} test/all_tests.cpp)
target_link_libraries(Test GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(Test)

# Make repl/main
add_executable(Repl ${SOURCES} src/main.cpp)
