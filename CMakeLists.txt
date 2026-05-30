cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(kaleidoscope VERSION 0.1)
add_executable(kaleidoscope src/main.cpp src/parser.cpp src/lexer.cpp src/codegen.cpp)
target_include_directories(kaleidoscope PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)