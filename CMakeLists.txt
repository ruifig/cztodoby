cmake_minimum_required(VERSION 3.27)

project(cztodoby)

set(ALL_FILES
    "tests.cpp"
    "TodoBy.h"
)

add_executable(cztodoby ${ALL_FILES})
set_target_properties(cztodoby PROPERTIES CXX_STANDARD 20)

