cmake_minimum_required(VERSION 3.10)

project(Script)

add_subdirectory(fmt)


add_executable(Host Main.cpp Hooks.cpp)
target_link_libraries(Host -ldl)
target_link_libraries(Host fmt::fmt)


target_compile_options(Host PRIVATE "-fvisibility=hidden")
target_link_options(Host PRIVATE -rdynamic)
set_property(TARGET Host PROPERTY CXX_STANDARD 17)


add_subdirectory(pybind11)
add_library(python SHARED Python.cpp)
target_link_libraries(python PRIVATE pybind11::embed)
