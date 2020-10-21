/**
 * @file dpatch/x64_code_generator.c
 *
 * Code generator for x64 CPUs.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#include "code_generator.h"
#include "status.h"

/**
 * Generate an opcode guaranteed to be undefined.
 *
 * @param result    Location to store the opcode.
 * @return          The success, or not, of the generation.
 */
dpatch_status generate_undefined_opcode(struct Opcode* result)
{
    result->length = 2;
    result->bytecode[0] = 0x0f;
    result->bytecode[1] = 0x0b;
    return DPATCH_STATUS_OK;
}