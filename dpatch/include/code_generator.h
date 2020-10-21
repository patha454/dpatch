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
 * x64 `Undefined Operation 2` opcode.
 *
 * Guaranteed to generate an `Illegal instruction` trap.
 */
const struct Opcode X64_UD2 = 
{
    2,  {0x0f, 0x0b}
};

#endif
