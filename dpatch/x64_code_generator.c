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
#include "machine_code.h"
#include "status.h"

/**
 * Append a guaranteed undefined opcode to a block of machine code.
 *
 * @param machine code
 * @return          The success, or not, of the generation.
 */
void append_undefined_opcode(machine_code_t machine_code)
{
    const uint8_t X64_UD2[] = {0x0f, 0x0b};
    machine_code_append_array(machine_code, 2, (uint8_t*) X64_UD2);
}

/**
 * Generate an opcode guaranteed to be undefined.
 *
 * @param machine_code Location to store the opcode.
 * @param addr      Address to jump to.
 */
void append_long_jump(machine_code_t machine_code, intptr_t addr)
{
    const uint8_t LJMP_OPCODE = 0xff;
    const uint8_t LJMP_MODRM_EXTENSION = 0x1 << 5;
    const uint8_t MODRM_RIP_RELATIVE = 0x5;
    /* The jump pointer is immediately after this instruction. */
    const uint32_t LJMP_RIP_DISPLACEMENT = 0x0;
    machine_code_append(machine_code, LJMP_OPCODE);
    machine_code_append(machine_code, LJMP_MODRM_EXTENSION | MODRM_RIP_RELATIVE);
    machine_code_append(machine_code, LJMP_RIP_DISPLACEMENT >> 0);
    machine_code_append(machine_code, LJMP_RIP_DISPLACEMENT >> 8);
    machine_code_append(machine_code, LJMP_RIP_DISPLACEMENT >> 16);
    machine_code_append(machine_code, LJMP_RIP_DISPLACEMENT >> 24);
    machine_code_append(machine_code, addr >> 0);
    machine_code_append(machine_code, addr >> 8);
    machine_code_append(machine_code, addr >> 16);
    machine_code_append(machine_code, addr >> 24);
    machine_code_append(machine_code, addr >> 32);
    machine_code_append(machine_code, addr >> 40);
    machine_code_append(machine_code, addr >> 48);
    machine_code_append(machine_code, addr >> 56);
}
