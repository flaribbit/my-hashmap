cmake_minimum_required(VERSION 3.0.0)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

project(lua-table VERSION 0.1.0)
add_library(${PROJECT_NAME} table.c)

project(lua-table-test VERSION 0.1.0)
add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} lua-table)
