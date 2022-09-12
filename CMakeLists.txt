cmake_minimum_required(VERSION 3.0.0)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

project(lua-table VERSION 0.1.0)
add_executable(${PROJECT_NAME} table.c)
