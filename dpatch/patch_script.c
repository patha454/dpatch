/**
 * @file dpatch/patch_script.c
 *
 *1 `patch_script.c` defines functions for parsing a
 * script containing patch operations to perform.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "patch_script.h"
#include "status.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * A patch script to be processed.
 */
struct patch_script
{
    /** The path to the script on disk. */
    char* script_path;
};

/**
 * Allocate and initialise a new patch script reference in
 * memory.
 *
 * @param new Location to store a handle to the new patch.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_script_new(patch_script_t** new)
{
    assert(new != NULL);
    patch_script_t* handle = malloc(sizeof*handle);
    *new = handle;
    if (handle == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    handle->script_path = NULL;
    return DPATCH_STATUS_OK;
}


/**
 * Deallocate a `patch_script_t` and free its resources.
 *
 * @param patch_script Handle to the patch_script to
 *      deallocate.
 */
void patch_script_free(patch_script_t* patch_script)
{
    assert(patch_script != NULL);
    if (patch_script->script_path != NULL)
    {
        free(patch_script->script_path);
    }
    free(patch_script);
}

/**
 * Set the path to a patch script to parse.
 *
 * @param patch_script Handle to the patch script to set.
 * @param path Path to the patch file to parse.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_script_patch
(
    patch_script_t* patch_script,
    char* path
)
{
    assert(patch_script != NULL);
    assert(path != NULL);
    patch_script->script_path = malloc(strlen(path));
    if (patch_script->script_path == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    strcpy(patch_script->script_path, path);
    return DPATCH_STATUS_OK;
}

