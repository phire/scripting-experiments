cmake_minimum_required(VERSION 3.10)

project(Script)
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(fmt)
add_subdirectory(json)


add_executable(Host Main.cpp Hooks.cpp)
target_link_libraries(Host -ldl)
target_link_libraries(Host fmt::fmt)



target_compile_options(Host PRIVATE "-fvisibility=hidden")
target_link_options(Host PRIVATE -rdynamic)
set_property(TARGET Host PROPERTY CXX_STANDARD 17)


#add_subdirectory(pybind11)
find_package(PythonLibs 3 REQUIRED)

add_library(python SHARED Python.cpp)
set_property(TARGET python PROPERTY CXX_STANDARD 17)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(python PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(python PRIVATE nlohmann_json::nlohmann_json)
