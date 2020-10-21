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
} dpatch_status;

#endif
