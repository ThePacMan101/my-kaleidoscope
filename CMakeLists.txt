cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(kaleidoscope VERSION 0.1)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(kaleidoscope ${SRC_FILES})
target_include_directories(kaleidoscope PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)