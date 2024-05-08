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
set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -g -std=c++20")
# inlude threading for gtest
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} src)

# set sources
set(SOURCES 
    "src/token.hpp" 
)
# Link runTests with what we want to test and the GTest and pthread library
add_executable(Lexer src/main.cpp )
target_link_libraries(Lexer ${GTEST_LIBRARIES} Threads::Threads)
