#include <dlfcn.h>
#include <link.h>
#include <stdint.h>
#include <stdlib.h>
#include <syslog.h>
#include "status.h"

#include "patch_set.h"
#include "patch_script.h"

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
    void (* target_start)(void) = NULL;
    patch_set_t* patch_set = NULL;
    patch_script_t* patch_script = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    target_handle = dlopen(NULL, RTLD_LAZY);
    if (target_handle == NULL)
    {
        syslog(LOG_ERR, "%s", dlerror());
        exit(EXIT_FAILURE);
    }

    if (patch_set_new(&patch_set) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "patch_set_t could not be initialised.");
        exit(EXIT_FAILURE);
    }
    if (patch_script_new(&patch_script) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "patch_script_t could not be initalised.");
        exit(EXIT_FAILURE);
    }
    if (patch_script_path(patch_script, "test.patch") != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Could not set patch script path.");
        exit(EXIT_FAILURE);
    }
    if (patch_script_parse(patch_script, patch_set) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Could not parse patch script");
        exit(EXIT_FAILURE);
    }

    if (patch_set_add_operation
        (
            patch_set,
            DPATCH_OP_REPLACE_FUNCTION_INTERNAL,
            PATCH_FROM,
            PATCH_TO
        ) != DPATCH_STATUS_OK
    )
    {
        syslog(LOG_ERR, "Could not add patch op");
        exit(EXIT_FAILURE);
    }
    if (patch_set_apply(patch_set) != DPATCH_STATUS_OK)
    {
        syslog(LOG_ERR, "Patch set could not be applied.");
    }
    patch_set_free(patch_set);
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
            "The target's '%s' symbol could not be located.",
            START_SYMBOL
        );
        exit(EXIT_FAILURE);
    }
}
