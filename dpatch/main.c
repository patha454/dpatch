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
    patch_set_t* patch_set = NULL;
    patch_script_t* patch_script = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    target_handle = dlopen(NULL, RTLD_LAZY);
    if (target_handle == NULL)
    {
        syslog(LOG_ERR, "%s", dlerror());
        exit(EXIT_FAILURE);
    }
    EXIT_ON_ERROR(patch_set_new(&patch_set));
    EXIT_ON_ERROR(patch_script_new(&patch_script));
    EXIT_ON_ERROR(patch_script_parse(patch_script, patch_set));
    LOG_ON_ERROR(patch_set_apply(patch_set));
    patch_set_free(patch_set);
    patch_script_free(patch_script);
}
