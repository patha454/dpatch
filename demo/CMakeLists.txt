cmake_minimum_required(VERSION 3.16)

project(
    dpatch_demos
    VERSION 0.0.0
    DESCRIPTION "Demonstration target programs `dpatch`."
    LANGUAGES C
)

add_executable(hello_world ${PROJECT_SOURCE_DIR}/hello_world.c)
add_executable(print_load_address ${PROJECT_SOURCE_DIR}/print_load_address.c)
add_executable(self_patch ${PROJECT_SOURCE_DIR}/self_patch.c)

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
set_property(TARGET print_load_address PROPERTY ENABLE_EXPORTS 1)
set_property(TARGET self_patch PROPERTY ENABLE_EXPORTS 1)

# Some compilers (Clang...) won't mark a program as `PT_DYN`, when the object
# enables exports, and requires dynamic loading. `dlopen` will requet any
# object not marked as `PT_DYN`
#
# Force the linker to generate a position independent excutable which must
# be PT_DYN.
target_link_options(hello_world PRIVATE "-pie")
target_link_options(print_load_address PRIVATE "-pie")
target_link_options(self_patch PRIVATE "-pie")

install(
    TARGETS
        hello_world
        print_load_address
        self_patch
    RUNTIME 
    DESTINATION
        dpatch/demo
)

