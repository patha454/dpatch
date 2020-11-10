/**
 * @file dpatch/patch.c
 *
 * `dpatch/patch.c` defines functions for manipulating and
 * applying a single patch.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "patch.h"
#include "status.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * A single patch operation to be applied to a target.
 *
 * The exact semantics of a patch operation will depend
 * on the `dpatch_operation` used.
 */
struct patch
{
    /** The new of the old symbol to replace. */
    char* old_symbol;

    /** The name of the symbol to substitute in. */
    char* new_symbol;

    /** The type of patch_set operation to apply. */
    dpatch_operation operation;
};

/**
 * Allocate and initialise a new patch in memory.
 *
 * @param new Location to store a handle to a new patch.
 * @return `DPATCH_STATUS_OK`, or an error on failure.
 */
dpatch_status patch_new(patch_t** new)
{
    assert(new != NULL);
    patch_t* handle = malloc(sizeof(struct patch));
    *new = handle;
    if (handle == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    handle->old_symbol = NULL;
    handle->new_symbol = NULL;
    handle->operation = DPATCH_OP_NOP;
    return DPATCH_STATUS_OK;
}

/**
 * Deallocate a `patch_t` and free its resources.
 *
 * @param patch Handle to the patch to deallocate.
 */
void patch_free(patch_t* patch)
{
    assert(patch != NULL);
    if (patch->old_symbol != NULL)
    {
        free(patch->old_symbol);
    }
    if (patch->new_symbol != NULL)
    {
        free(patch->new_symbol);
    }
    free(patch);
}

/**
 * Configure a patch with an operation to perform.
 *
 * @param patch Handle to the `patch_t` to configure.
 * @param op Patch operation to perform.
 * @param old_sym Old symbol to be replaced.
 * @param new_sym New symbol to patch in.
 */
dpatch_status patch_operation
(
    patch_t* patch,
    dpatch_operation op,
    char* old_sym,
    char* new_sym
)
{
    assert(patch != NULL);
    patch->operation = op;
    patch->old_symbol = malloc(strlen(old_sym) + 1);
    if (patch->old_symbol == NULL)
    {
        return DPATCH_STATUS_ENOMEM;
    }
    patch->new_symbol = malloc(strlen(new_sym) + 1);
    if (patch->new_symbol == NULL)
    {
        free (patch->old_symbol);
        return DPATCH_STATUS_ENOMEM;
    }
    strcpy(patch->new_symbol, new_sym);
    strcpy(patch->old_symbol, old_sym);
    return DPATCH_STATUS_OK;
}

/**
 * Apply a patch to replace a function inside the same object.
 *
 * @param patch Handle to the patch to apply.
 */
dpatch_status patch_replace_function_internal(patch_t* patch)
{
    assert(patch != NULL);
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
    if
    (
        (status = machine_code_new(&machine_code)) != 
        DPATCH_STATUS_OK
    )
    {
        return status;
    }
    if
    (
        (status = append_long_jump(machine_code, patch_to)) !=
        DPATCH_STATUS_OK
    )
    {
        return status;
    }
    if
    (
        (status = machine_code_insert(machine_code, patch_from)) !=
        DPATCH_STATUS_OK
    )
    {
        return status;
    }
    machine_code_free(machine_code);
    return DPATCH_STATUS_OK;
}

/**
 * Attempt to apply a patch to the running program.
 *
 * @param patch Handle to the patch to apply.
 * @return `DPATCH_STATUS_OK` or an error on failure.
 */
dpatch_status patch_apply(struct patch* patch)
{
    assert(patch != NULL);
    switch (patch->operation)
    {
        case DPATCH_OP_REPLACE_FUNCTION_INTERNAL:
            return patch_replace_function_internal(patch);
            break;
        case DPATCH_OP_NOP:
            return DPATCH_STATUS_OK;
            break;
        default:
            return DPATCH_STATUS_EUNKNOWN;
    }
}
