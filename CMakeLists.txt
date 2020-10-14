cmake_minimum_required(VERSION 3.7)

project(
    dpatch
    VERSION 0.0.0
    DESCRIPTION "Experimental dynamic software patching tools."
    LANGUAGES C
)

add_subdirectory(dpatch)
add_subdirectory(tests)
