#include <dlfcn.h>
#include <link.h>
#include <stdint.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include "code_generator.h"
#include "status.h"

#include "patch_set.h"

#define PROGRAM_IDENT "dpatch"
#define START_SYMBOL "main"
#define PATCH_FROM "alpha"
#define PATCH_TO "bravo"

/**
 * Suppresses unused parameter warnings.
 *
 * @note `UNUSED` is intended for use in function bodies
 * when a parameter is required by an API, but is unused.
 *
 * `UNUSED` generates a synaxically correct, but
 * meaningless statement, using the variable, making it
 * technically 'used' to suppress the warning.
 */
#define UNUSED(x) ((void)(x))

/**
 * Inidcate the verion of the link audit library this tool
 * was compiled against.
 * 
 * `la_version` allows the RTDL to reect this audit library
 * at runtime if it uses an unsupported ABI.
 *
 * @param version The RTDL Audit API the host supports.
 * @return The RTDL audit API this object was compiled
 * for.
 */
extern unsigned int la_version(unsigned int version)
{
    if (version != LAV_CURRENT)
    {
        openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
        syslog(LOG_ERR, "Unsupported RTLD audit API. Host: v%d, dpatch: v%d\n", version, LAV_CURRENT);
        closelog();
        exit(EXIT_FAILURE);
    }
    return LAV_CURRENT;
}

/**
 * Preinit hook to be called before the target's `main` is
 * executed.
 *
 * For now, the preinit hook patches out a function before
 * the program is running.
 *
 * @param cookie The object at the head of the link map.
 */
extern void la_preinit(uintptr_t* cookie)
{
    UNUSED(cookie);
    void* target_handle = NULL;
    /*void* patch_from = NULL;
    void* patch_to = NULL; */
    void (* target_start)(void) = NULL;
    machine_code_t* machine_code = NULL;
    patch_set_t* patch_set = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    if (machine_code_new(&machine_code) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "machine_code_t could not be initialised.");
        closelog();
        exit(EXIT_FAILURE);
    }
    target_handle = dlopen(NULL, RTLD_LAZY);
    if (target_handle == NULL)
    {
        syslog(LOG_ERR, "%s", dlerror());
        exit(EXIT_FAILURE);
    }

    if (patch_set_new(&patch_set) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "patch_set_t could not be initialised.");
    }

    if (patch_set_add_operation
        (
            patch_set,
            DPATCH_OP_REPLACE_FUNCTION_INTERNAL,
            "alpha",
            "bravo"
        ) != DPATCH_STATUS_OK
    )
    {
        syslog(LOG_ERR, "Could not add patch op");
    }
    if (patch_set_apply(patch_set) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Patch set could not be applied.");
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
    /*patch_from = (void (*)(void)) dlsym(target_handle, PATCH_FROM);
    patch_to = (void (*)(void)) dlsym(target_handle, PATCH_TO); */
    #pragma GCC diagnostic pop
    if (target_start == NULL)
    {
        syslog(
            LOG_ERR,
            "The target's '%s' symbol could not be located.",
            START_SYMBOL
        );
        exit(EXIT_FAILURE);
    }
    /*
    if (append_long_jump(machine_code, (intptr_t) patch_to) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Failure generating a long jump operation.");
    }
    if (machine_code_insert(machine_code, (intptr_t) patch_from) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Failure inserting new machine code.");
    } */
    target_start();
    /*machine_code_free(machine_code); */
    patch_set_free(patch_set);
    closelog();
    exit(EXIT_SUCCESS);
}
