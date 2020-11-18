/**
 * @file `dpatch/include/status.h`
 *
 * `status.h` defines status codes used by dpatch.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#ifndef DPATCH_INCLUDE_STATUS_H_
#define DPATCH_INCLUDE_STATUS_H_

/**
 * Indicates the success or failure of an operation by dpatch.
 */
typedef enum
{
    /** Success. */
    DPATCH_STATUS_OK,

    /** General or unspecified error. */
    DPATCH_STATUS_ERROR,

    /** Memory (re)/allocation failed. */
    DPATCH_STATUS_ENOMEM,

    /** Failed to modify memory protection. */
    DPATCH_STATUS_EMPROT,

    /** Unsupported or unknown patch operation. */
    DPATCH_STATUS_EUNKNOWN,

    /** Error accessing dynamic symbols with `dlfcn.h` */
    DPATCH_STATUS_EDYN,

    /** File I/O error. */
    DPATCH_STATUS_EFILE,

    /** Script parsing error. */
    DPATCH_STATUS_ESYNTAX
} dpatch_status;

/**
 * Get a human readable string describing a status code.
 *
 * @param status Status code to translate.
 * @return Pointer to a string describing the error.
 */
const char* str_status(dpatch_status status);

#endif
