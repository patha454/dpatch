cmake_minimum_required(VERSION 3.16)

project(
    dpatch_executable
    VERSION 0.0.0
    DESCRIPTION "Experimental dynamic software updating framework."
    LANGUAGES C
)

add_executable(dpatch ${PROJECT_SOURCE_DIR}/main.c)

set_property(TARGET dpatch PROPERTY C_STANDARD 99)

target_link_libraries(dpatch PUBLIC ${CMAKE_DL_LIBS})

target_include_directories(dpatch PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(
    dpatch PRIVATE
    "SHELL:-W"
    "SHELL:-Wall"
    "SHELL:-Wextra"
    "SHELL:-Werror"
    "SHELL:-pedantic"
)

install(TARGETS dpatch RUNTIME)
