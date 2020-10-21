#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/mman.h>
#include <unistd.h>
#include "code_generator.h"
#include "status.h"
#include "metamorphosis.h"

#define PROGRAM_IDENT "dpatch"
#define START_SYMBOL "main"

/**
 * x64 `Undefined Operation 2` opcode.
 *
 * Guaranteed to generate an `Illegal instruction` trap.
 */
const struct Opcode X64_UD2 = 
{
    2,  {0x0f, 0x0b}
};

int main(int argc, char** argv)
{
    char* target = argv[1];
    void* target_handle = NULL;
    void (* target_start)(void) = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    if (argc < 2)
    {
        syslog(LOG_ERR, "A target program must be provided as an argument.");
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
    insert_op((intptr_t) target_start, X64_UD2);
    target_start();
    closelog();
    exit(EXIT_SUCCESS);
}
