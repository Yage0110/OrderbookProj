# CMakeLists.txt
# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(OPTIONSTRADINGBOOK)

# Enable testing
enable_testing()

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the path for GoogleTest
set(GTEST_ROOT "/usr/local/opt/googletest")

# Include GoogleTest directories
include_directories("${GTEST_ROOT}/include")

# Link GoogleTest libraries
link_directories("${GTEST_ROOT}/lib")

# Add the executable for your tests
add_executable(OPTIONSTRADINGBOOK test_include.cpp Orderbook.cpp)

# Link with GoogleTest and pthread
target_link_libraries(OPTIONSTRADINGBOOK gtest gtest_main pthread)

# Add tests to CTest
add_test(NAME OPTIONSTRADINGBOOK COMMAND OPTIONSTRADINGBOOK)