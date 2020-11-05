/**
 * @file dpatch/include/patch.h
 *
 * `patch.h` declares a `patch_t` for outlining the
 * the contents of a patch, and provides functions
 * for operating on a `patch_t`.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#ifndef DPATCH_INCLUDE_PATCH_H_
#define DPATCH_INCLUDE_PATCH_H_

#include "status.h"

/**
 * patch_t provides an opaque type for representing a patch
 * to be applied to a program.
 */
typedef struct patch patch_t;

/**
 * A type of patch operation to perform.
 */
typedef enum
{
    /** Replace a function. */
    DPATCH_OP_REPLACE_FUNCTION
} dpatch_operation;

/**
 * Allocates and initialises a new patch.
 *
 * @param patch Pointer to address for storing the new
 *              patch handel.
 * @return `DPATCH_STATUS_SUCCESS`, or an error on failure.
 */
dpatch_status patch_new(patch_t** patch);

/**
 * Free and deallocate a patch.
 *
 * @param patch Handle to the patch to free.
 * @return `DPATCH_STATUS_SUCCESS`, or an error code on
 *         failure.
 */
dpatch_status patch_free(patch_t* patch);

/**
 * Add a patch operation to the set of operations
 * associated with a patch.
 *
 * @note The exact semantics of the `old` and `new` symbol
 * parameters will depend on the patch operation `op`
 * selected. `old` is generally the symbol to be replaced
 * and `new` is the new symbol to replace `old`.
 *
 * @param patch Handle to the patch an operation to.
 * @param op Patch operation to perform.
 * @param old Symbol to be updated.
 * @param new Symbol to update to.
 * @return `DPATCH_STATUS_SUCCESS`, or an error on failure.
 */
dpatch_status patch_add_operation
(
    patch_t* patch,
    dpatch_operation op,
    char* old,
    char* new
);

/**
 * Attempt to apply a patch to the hosted program.
 *
 * @param patch Handle to the patch to be applied.
 * @return `DPATCH_STATUS_SUCCESS`, or an error on failure.
 */
dpatch_status patch_apply(patch_t* patch);

#endif
