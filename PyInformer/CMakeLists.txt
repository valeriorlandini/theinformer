cmake_minimum_required(VERSION 3.6...3.18)
project(pyinformer)

add_subdirectory(pybind11)
pybind11_add_module(pyinformer src/main.cpp)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(pyinformer
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
