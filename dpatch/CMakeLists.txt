cmake_minimum_required(VERSION 3.7)

project(
    dpatch_executable
    VERSION 0.0.0
    DESCRIPTION "Experimental dynamic software updating framework."
    LANGUAGES C
)

add_executable(dpatch main.c)
target_link_libraries(dpatch dl)