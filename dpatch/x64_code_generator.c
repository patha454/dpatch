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

/**
 * Generate an opcode guaranteed to be undefined.
 *
 * @param result    Location to store the opcode.
 * @param addr      Address to jump to.
 * @return          The success, or not, of the generation.
 */
dpatch_status generate_long_jump(struct Opcode* result, intptr_t addr)
{
    const uint8_t LJMP_OPCODE = 0xff;
    const uint8_t LJMP_MODRM_EXTENSION = 0x1 << 5;
    const uint8_t MODRM_RIP_RELATIVE = 0x5;
    /* The jump pointer is immediately after this instruction. */
    const uint32_t LJMP_RIP_DISPLACEMENT = 0x0;
    result->bytecode[0] = LJMP_OPCODE;
    result->bytecode[1] = LJMP_MODRM_EXTENSION | MODRM_RIP_RELATIVE;
    result->bytecode[2] = LJMP_RIP_DISPLACEMENT >> 0;
    result->bytecode[3] = LJMP_RIP_DISPLACEMENT >> 8;
    result->bytecode[4] = LJMP_RIP_DISPLACEMENT >> 16;
    result->bytecode[5] = LJMP_RIP_DISPLACEMENT >> 24;
    result->bytecode[6] = addr >> 0;
    result->bytecode[7] = addr >> 8;
    result->bytecode[8] = addr >> 16;
    result->bytecode[9] = addr >> 24;
    result->bytecode[10] = addr >> 32;
    result->bytecode[11] = addr >> 40;
    result->bytecode[12] = addr >> 48;
    result->bytecode[13] = addr >> 56;
    result->length = 14;
    return DPATCH_STATUS_OK;
}
