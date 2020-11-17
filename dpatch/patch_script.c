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
#include <stdio.h>
#include <string.h>

#define PATCH_SCRIPT_MAX_LINE_LEN 255

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
dpatch_status patch_script_path
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

/**
 * Parse a patch script into memory.
 *
 * @param patch_script Handle to the patch script to parse.
 * @param patch_set Set to parse the script into.
 * @return `DPATCH_STATUS_OK` or an error code.
 */
dpatch_status patch_script_parse
(
    patch_script_t* patch_script,
    patch_set_t* patch_set
)
{
    (void) patch_script;
    (void) patch_set;
    FILE* script = fopen(patch_script->script_path, "r");
    char line[PATCH_SCRIPT_MAX_LINE_LEN];
    if (script == NULL)
    {
        return DPATCH_STATUS_EFILE;
    }
    while (fscanf(script, "%s\n", line) == 1)
    {
        printf("%s\n", line);
    }
    return DPATCH_STATUS_ERROR;
}
