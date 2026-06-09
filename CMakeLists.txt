cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(kaleidoscope VERSION 0.1)

# setup correct Intellisense in VScode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs Core Support Analysis)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(kaleidoscope ${SRC_FILES})
target_include_directories(kaleidoscope PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(kaleidoscope PRIVATE ${llvm_libs})