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

#include <dlfcn.h>
#include "code_generator.h"

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

/**
 * Grow the memory allocated to a patch set.
 *
 * @param patch_set Handle to the patch set to grow.
 * @return `DPATCH_STATUS_OK` on success, or an error.
 */
dpatch_status patch_set_grow(patch_set_t* patch_set)
{
    struct patch* realloc_result = NULL;
    assert(patch_set != NULL);
    patch_set->allocated_length *= 2;
    realloc_result = realloc
    (
        patch_set->patches,
        sizeof(struct patch) * patch_set->allocated_length
    );
    if (realloc_result == NULL)
    {
        patch_set->allocated_length /= 2;
        return DPATCH_STATUS_ENOMEM;
    }
    patch_set->patches = realloc_result;
    return DPATCH_STATUS_OK;
}

/**
 * Add a patch operation to the set of operations
 * associated with a patchset.
 *
 * @note The exact semantics of the `old` and `new` symbol
 * parameters will depend on the patch operation `op`
 * selected. `old` is generally the symbol to be replaced
 * and `new` is the new symbol to replace `old`.
 *
 * @param patch_set Handle to the patch_set an operation to.
 * @param op Patch operation to perform.
 * @param old Symbol to be updated.
 * @param new Symbol to update to.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_add_operation
(
    patch_set_t* patch_set,
    dpatch_operation op,
    char* old,
    char* new
)
{
    struct patch* new_patch = NULL;
    dpatch_status status = DPATCH_STATUS_OK;
    if (patch_set->length == patch_set->allocated_length)
    {
        status = patch_set_grow(patch_set);
        if (status != DPATCH_STATUS_OK)
        {
            return status;
        }
    }
    new_patch = &patch_set->patches[patch_set->length++];
    new_patch->old_symbol = old;
    new_patch->new_symbol = new;
    new_patch->operation = op;
    return DPATCH_STATUS_OK;
}

dpatch_status patch_replace_function_internal(struct patch* patch)
{
    (void) patch;
    intptr_t patch_from = (intptr_t) NULL;
    intptr_t patch_to = (intptr_t) NULL;
    void* process_handle = dlopen(NULL, RTLD_LAZY);
    machine_code_t* machine_code = NULL;
    dpatch_status status = DPATCH_STATUS_OK;
    if (process_handle == NULL)
    {
        return DPATCH_STATUS_EDYN;
    }
    patch_from = (intptr_t) dlsym(process_handle, patch->old_symbol);
    patch_to = (intptr_t) dlsym(process_handle, patch->new_symbol);
    if (patch_from == (intptr_t) NULL || patch_to == (intptr_t) NULL)
    {
        return DPATCH_STATUS_EDYN;
    }
    if ((status = machine_code_new(&machine_code)) != DPATCH_STATUS_OK)
    {
        return status;
    }
    if ((status = append_long_jump(machine_code, patch_to)) != DPATCH_STATUS_OK)
    {
        return status;
    }
    if ((status = machine_code_insert(machine_code, patch_from)) != DPATCH_STATUS_OK)
    {
        return status;
    }
    machine_code_free(machine_code);
    return DPATCH_STATUS_OK;
}

dpatch_status patch_apply(struct patch* patch)
{
    switch (patch->operation)
    {
        case DPATCH_OP_REPLACE_FUNCTION_INTERNAL:
            return patch_replace_function_internal(patch);
            break;
        default:
            return DPATCH_STATUS_EUNKNOWN;
    }
}

/**
 * Attempt to apply a patch_set to the target program.
 *
 * @param patch_set Handle to the patch_set to be applied.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_set_apply(patch_set_t* patch_set)
{
    size_t i = 0;
    dpatch_status status = DPATCH_STATUS_OK;
    for (i = 0; i < patch_set->length; i++)
    {
        status = patch_apply(&patch_set->patches[i]);
        if (status != DPATCH_STATUS_OK)
        {
            return status;
        }
    }
    return DPATCH_STATUS_OK;
}
