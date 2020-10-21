/**
 * @file dpatch/include/metamorphosis.h
 *
 * `metamorphosis.h` declears functions for modifying
 * binary segments loaded in memory.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#ifndef DPATCH_INCLUDE_METAMORPHOSIS_H_
#define DPATCH_INCLUDE_METAMORPHOSIS_H_

#include <stdint.h>
#include "status.h"

/**
 * Changes the memory protection as needed, adjusting `addr` and `len` to meet
 * the page boundary requirements of `mprotect` if required.
 *
 * @param addr  Location to modify permissions for.
 * @param len   Length of the segment to adjust.
 * @param prot  The new memory protection mode bits.
 * @return      The success (or not) or the operation.
 */
dpatch_status mprotect_round(intptr_t addr, size_t len, int prot);

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
dpatch_status insert_op(intptr_t addr, struct Opcode op);

#endif
