/**
 * @file dpatch/metamorphosis.c
 *
 * `metamorphosis.c` implements functions for changing data loaded
 * into memory segments.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <syslog.h>
#include <sys/mman.h>
#include <unistd.h>
#include "status.h"
#include "code_generator.h"

/**
 * Changes the memory protection as needed, adjusting `addr` and `len` to meet
 * the page boundary requirements of `mprotect` if required.
 *
 * @param addr  Location to modify permissions for.
 * @param len   Length of the segment to adjust.
 * @param prot  The new memory protection mode bits.
 * @return      The success (or not) or the operation.
 */
dpatch_status mprotect_round(intptr_t addr, size_t len, int prot)
{
    intptr_t delta = 0;
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1)
    {
        syslog(LOG_ERR, "The page size could not be detected using `sysconf`.");
        return DPATCH_STATUS_ERROR;
    }
    delta = addr % page_size;
    addr -= delta;
    len += delta;
    #pragma message "`mprotect` on memory not acquired by `mmap` is non-POSIX compliant Linux extension."
    if (mprotect((void*) addr, len, prot) == -1)
    {
        syslog(LOG_ERR, "The memory protection could not be modified.");
        return DPATCH_STATUS_ERROR;
    }
    return DPATCH_STATUS_OK;
}

/**
 * Insert an opcode into the program segment.
 *
 * @note `insert_op` does not check the address is a valid range, or even in
 * the code segment. `insert_op` assumes the target address range is in the
 * code segment when it resets the memory permissions to `PROT_READ |
 * PROT_EXEC`. If the target is not in the code segment, you may get set faults
 * if later code assumes the target address is writable.
 *
 * @param addr  The address to insert code into.
 * @param op    The bytecode to insert into the segment.
 * @return      The success of the operation, or an error code.
 */
dpatch_status insert_op(intptr_t addr, struct Opcode op)
{
    dpatch_status status = DPATCH_STATUS_OK;
    status = mprotect_round(addr, op.length, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (status != DPATCH_STATUS_OK)
    {
        return status;
    }
    memcpy((void*) addr, &op.bytecode, op.length);
    status = mprotect_round(addr, op.length, PROT_READ | PROT_EXEC);
    if (status != DPATCH_STATUS_OK)
    {
        return status;
    }
    return DPATCH_STATUS_OK;
}
