/**
 * @file dpatch/patch_set.c
 *
 * `patch_set.c` defines functions for generating and
 * applying a set of patches.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "patch_set.h"
#include "status.h"
#include <assert.h>
#include <stdlib.h>

#define PATCH_DEFAULT_LENGTH 8

/**
 * A single patch operation to be applied to a target.
 *
 * The exact semantics of a patch operation will depend
 * on the `dpatch_operation` used.
 */
struct patch
{
    /** The old symbol to be replaced. */
    char* old_symbol;

    /** The new symbol to replace the old symbol. */
    char* new_symbol;

    /** The type of patch_set operation to apply. */
    dpatch_operation operation;
};

struct patch_set
{
    /** The number of `patches` allocated in memory. */
    size_t allocated_length;

    /** The number of patches in the patch set. */
    size_t length;

    /** The patches to be applied. */
    struct patch* patches;
};

/**
 * Allocates and initialises a new patch_set.
 *
 * @param patch_set Pointer to address for storing the new
 *              patch_set handel.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_new(patch_set_t** patch_set)
{
    patch_set_t* new_set = malloc(sizeof(struct patch_set));
    *patch_set = new_set;
    if (*patch_set == NULL)
    {
        return DPATCH_STATUS_ENOMEM; 
    }
    new_set->length = 0;
    new_set->allocated_length = PATCH_DEFAULT_LENGTH;
    new_set->patches = malloc(sizeof(struct patch) * new_set->allocated_length);
    if (new_set->patches == NULL)
    {
        patch_set_free(new_set);
        *patch_set = NULL;
        return DPATCH_STATUS_ENOMEM;
    }
    return DPATCH_STATUS_OK;
}

/**
 * Free and deallocate a patch set.
 *
 * @param patch_set Handle to the patch set to free.
 */
void patch_set_free(patch_set_t* patch_set)
{
    assert(patch_set != NULL);
    if (patch_set->patches != NULL)
    {
        free(patch_set->patches);
    }
    patch_set->patches = NULL;
    patch_set->allocated_length = 0;
    patch_set->length = 0;
    free(patch_set);
}
