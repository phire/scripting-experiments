cmake_minimum_required(VERSION 3.10)

project(Script)
add_executable(Host Main.cpp)
target_link_libraries(Host -ldl)

add_subdirectory(pybind11)
add_library(python SHARED Python.cpp)
target_link_libraries(python PRIVATE pybind11::embed)