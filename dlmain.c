#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    char* target = argv[1];
    void* target_handle = NULL;
    if (argc < 2)
    {
        fprintf(stderr, "%s: Requires a target program as an argument.\n", __FILE__);
        return EXIT_FAILURE;
    }
    target_handle = dlopen(target, RTLD_LAZY);
    printf("Target handle: 0x%p\n", target_handle);
    return EXIT_SUCCESS;
}