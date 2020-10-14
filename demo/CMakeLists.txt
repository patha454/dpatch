cmake_minimum_required(VERSION 3.7)

project(
    dpatch_demos
    VERSION 0.0.0
    DESCRIPTION "Demonstration target programs `dpatch`."
    LANGUAGES C
)

add_executable(hello_world ${PROJECT_SOURCE_DIR}/hello_world.c)

# `dpatch's` targets must expose all their symbols as dynamic symbols so
# `dpatch can manipulate them with `dlopen` and friends, as well as to
# easily generate backtraces for checking function activeness.
#
# These are limitations of the `libdl`, which we accept for speed of
# implementation. Hypothetically, a comparable loader-style patching
# tool could also use the static symbol stable in a program to avoid
# this limitation. We ignore this for now because we are focused on
# writing dynamic patching demonstrations, not run time dynamic
# linker-loaders.
set_property(TARGET hello_world PROPERTY ENABLE_EXPORTS 1)

install(TARGETS hello_world RUNTIME DESTINATION dpatch/demo)

