cmake_minimum_required(VERSION 3.10)
project(natacha VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(natacha_lib src/core/neuronio.cpp)
target_include_directories(natacha_lib PUBLIC include)

add_executable(natacha src/main.cpp)
target_link_libraries(natacha PRIVATE natacha_lib)

# Tests
enable_testing()
add_executable(test_neuronio tests/test_neuronio.cpp)
target_link_libraries(test_neuronio PRIVATE natacha_lib)
add_test(NAME test_neuronio COMMAND test_neuronio)
