#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include "code_generator.h"
#include "status.h"

#define PROGRAM_IDENT "dpatch"
#define START_SYMBOL "main"
#define PATCH_FROM "alpha"
#define PATCH_TO "bravo"

int main(int argc, char** argv)
{
    char* target = argv[1];
    void* target_handle = NULL;
    void* patch_from = NULL;
    void* patch_to = NULL;
    void (* target_start)(void) = NULL;
    machine_code_t machine_code = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    if (argc < 2)
    {
        syslog(LOG_ERR, "A target program must be provided as an argument.");
        closelog();
        exit(EXIT_FAILURE);
    }
    if (machine_code_new(&machine_code) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "machine_code_t could not be initialised.");
        closelog();
        exit(EXIT_FAILURE);
    }
    target_handle = dlopen(target, RTLD_LAZY);
    if (target_handle == NULL)
    {
        syslog(LOG_ERR, "dlopen could not load the target '%s.'", target);
        exit(EXIT_FAILURE);
    }
    /*
     * Casting an object pointer to a function pointer is not strictly valid
     * ANSI C, because some machines have diffrent data and instruction pointer
     * widths. Diffrent data and instruction widths is largely an archaic detail
     * and the design of `dlfcn` forces us to make the cast. Disable pedantic 
     * errors for the cast.
     */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    target_start = (void (*)(void)) dlsym(target_handle, START_SYMBOL);
    patch_from = (void (*)(void)) dlsym(target_handle, PATCH_FROM);
    patch_to = (void (*)(void)) dlsym(target_handle, PATCH_TO);
    #pragma GCC diagnostic pop
    if (target_start == NULL)
    {
        syslog(
            LOG_ERR,
            "The '%s:%s' symbol could not be located.",
            target,
            START_SYMBOL
        );
        exit(EXIT_FAILURE);
    }
    if (append_long_jump(machine_code, (intptr_t) patch_to) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Failure generating a long jump operation.");
    }
    if (machine_code_insert(machine_code, (intptr_t) patch_from) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Failure inserting new machine code.");
    }
    target_start();
    machine_code_free(machine_code);
    closelog();
    exit(EXIT_SUCCESS);
}
