/**
 * @file dpatch/include/machine_code.h
 *
 * `machine_code.h` defines a `machine_code_t` representing
 * a block of executable binary code, and declares
 * functions for manipulating the binary and injecting it
 * into memory.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#ifndef DPATCH_INCLUDE_MACHINE_CODE_H_
#define DPATCH_INCLUDE_MACHINE_CODE_H_

#include <stddef.h>
#include <stdint.h>

/**
 * `machine_code_t` is a handle to a container for machine
 * executable binary code.
 */
typedef struct machine_code* machine_code_t;

/**
 * Allocate and initialise a new machine code container.
 *
 * @return A handle to a machine code container, or `NULL`
 *         on failure.
 */
machine_code_t machine_code_new();

/**
 * Deallocate a machine code container and its contents.
 *
 * @param machine_code Handle to free.
 */
void machine_code_free(machine_code_t machine_code);

/**
 * Get the length, in bytes, of binary stored in the
 * container referenced by handle `machine_code`.
 *
 * @param machine_code Handle to get the length of.
 * @return The length, in bytes, of the `machine_code`.
 */
size_t machine_code_length(machine_code_t machine_code);

/**
 * Append a byte to the machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param byte A byte to append to the machine code.
 */
void machine_code_append(machine_code_t machine_code, uint8_t byte);

/**
 * Append bytes to the end of machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param length Number of bytes to append to the machine code.
 * @param bytes Array of bytes to append
 */
void machine_code_append_array(machine_code_t machine_code, size_t length, uint8_t bytes[]);

/**
 * Insert machine code into a program segment.
 *
 * @param machine_code Handle to the machine code to insert.
 * @param address Address to write the code into.
 */
void machine_code_insert(machine_code_t machine_code, intptr_t address);

#endif
