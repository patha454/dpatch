cmake_minimum_required(VERSION 3.7)

project(
    dpatch_executable
    VERSION 0.0.0
    DESCRIPTION "Experimental dynamic software updating framework."
    LANGUAGES C
)

add_executable(dpatch ${PROJECT_SOURCE_DIR}/main.c)

set_property(TARGET dpatch PROPERTY C_STANDARD 99)

target_link_libraries(dpatch PUBLIC ${CMAKE_DL_LIBS})

add_compile_options(-W -Wall -Wextra -Werror -pedantic -std=c99)

install(TARGETS dpatch RUNTIME)
