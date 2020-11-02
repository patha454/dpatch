/**
 * @file dpatch/include/code_generator.h
 *
 * `code_generator.h` defines an interface to a system which can
 * generate machine code for a target architecture.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date October 2020.
 */

#ifndef DPATCH_INCLUDE_CODE_GENERATOR_H_
#define DPATCH_INCLUDE_CODE_GENERATOR_H_

#include <stdint.h>
#include "status.h"
#include "machine_code.h"

/** Maximum supported opcode length, in bytes.
 *
 * @note This is arbitrary, but matched to x64's maximum
 * legal opcode length.
 */
#define DPATCH_MAX_OPCODE_LEN 15

/**
 * A (potentially variable length) binary instruction.
 */
struct Opcode
{
    /** Length of the instruction, in bytes. */
    uint8_t length;

    /**
     * Instruction stored little endian.
     * 
     * The max length of an x64 instruction is 15 bytes.
     */
    uint8_t bytecode[DPATCH_MAX_OPCODE_LEN];
};

/**
 * Append a guaranteed undefined opcode to a block of machine code.
 *
 * @param machine code
 * @return          The success, or not, of the generation.
 */
void append_undefined_opcode(machine_code_t machine_code);

/**
 * Generate an opcode guaranteed to be undefined.
 *
 * @param result    Location to store the opcode.
 * @param addr      Address to jump to.
 * @return          The success, or not, of the generation.
 */
dpatch_status generate_long_jump(struct Opcode* result, intptr_t addr);

#endif
