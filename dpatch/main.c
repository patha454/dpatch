#include <assert.h>
#include <dlfcn.h>
#include <link.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include "status.h"

#include <stdio.h>

#include "patch_set.h"
#include "patch_script.h"

#define PROGRAM_IDENT "dpatch"

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

patch_set_t* pending_patch = NULL;

bool patch_pending = false;

/**
 * Applies a pending patch.
 *
 * @warning `do_patch` assumes a patch has been parsed.
 *
 * The patch parsing functions are not reentrant, so
 * they must be called outside of the signal handler,
 * or made reentrant.
 */
dpatch_status do_patch()
{
    printf("Doing path...\n");
    return DPATCH_STATUS_OK;
    /*assert(pending_patch != NULL);
    LOG_ON_ERROR(patch_set_apply(pending_patch));
    patch_set_free(pending_patch);
    pending_patch = NULL;
    return DPATCH_STATUS_OK; */
}

void* patch_loop()
{
    while (true)
    {
        if (patch_pending)
        {
            patch_pending = false;
            do_patch();
        }
        sleep(1);
    }
    return NULL;
}

/**
 * SIGUSR2 initiates a dynamic path.
 *
 * @param signal The incomming singal to handle.
 */
void sigusr2_handler(int signal)
{
    assert(signal == SIGUSR2);
    syslog(LOG_INFO, "Recieved SIGUSR2. Requesting patch.");
    patch_pending = true;
}

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
        syslog(LOG_ERR, "Unsupported RTLD audit API. Host: v%d, dpatch: v%d", version, LAV_CURRENT);
        closelog();
        exit(EXIT_FAILURE);
    }
    return LAV_CURRENT;
}

/**
 * Preinit hook to be called before the target's `main` is
 * executed.
 *
 * The pre-init hook sets up the singal handler for dynamic
 * patching.
 *
 * @param cookie The object at the head of the link map.
 */
extern void la_preinit(uintptr_t* cookie)
{
    UNUSED(cookie);
    pthread_t patch_thread;
    patch_script_t* patch_script = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    signal(SIGUSR2, sigusr2_handler);
    EXIT_ON_ERROR(patch_set_new(&pending_patch));
    EXIT_ON_ERROR(patch_script_new(&patch_script));
    EXIT_ON_ERROR(patch_script_parse(patch_script, pending_patch));
    patch_script_free(patch_script);
    pthread_create(&patch_thread, NULL, &patch_loop, NULL);
}
