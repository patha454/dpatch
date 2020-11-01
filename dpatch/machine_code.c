/**
 * @file dpatch/machine_code.c
 *
 * `machine_code.h` defines functions for manipulating
 * blocks of machine executable binary code, and injecting
 * them into process memory.
 *
 * @author H Paterson.
 * @copyright BSL-1.0.
 * @date November 2020.
 */

#include "machine_code.h"
#include <assert.h>
#include <stdlib.h>

#define MACHINE_CODE_DEFAULT_LEN 8

/**
 * Storage for variable length chunks of executable binary.
 */
struct machine_code
{
    /** Length of the binary stored. */
    size_t length;

    /** Length of memory allocated to the binary. */
    size_t allocated_length;

    /** The binary data stored. */
    uint8_t* binary;
};

/**
 * Allocate and initialise a new machine code container.
 *
 * @return A handle to a machine code container, or `NULL`
 *         on failure.
 */
machine_code_t machine_code_new()
{
    machine_code_t new = malloc(sizeof *new);
    if (new == NULL)
    {
        return NULL;
    }
    new->length = 0;
    new->allocated_length = MACHINE_CODE_DEFAULT_LEN;
    new->binary = malloc(MACHINE_CODE_DEFAULT_LEN);
    if (new->binary == NULL)
    {
        machine_code_free(new);
        return NULL;
    }
    return new;
}

/**
 * Deallocate a machine code container and its contents.
 *
 * @param machine_code Handle to free.
 */
void machine_code_free(machine_code_t machine_code)
{
    if (machine_code == NULL)
    {
        return;
    }
    if (machine_code->binary)
    {
        free(machine_code->binary);
    }
    machine_code->binary = NULL;
    machine_code->allocated_length = 0;
    machine_code->length = 0;
    free(machine_code);
}

/**
 * Grow the memory allocated to a machine code container.
 *
 * @param machine_code Handle to the container to grow.
 */
void machine_code_grow(machine_code_t machine_code)
{
    machine_code->allocated_length *= 2;
    machine_code->binary = realloc(machine_code->binary, machine_code->allocated_length);
    assert(machine_code->binary != NULL);
}

/**
 * Get the length, in bytes, of binary stored in the
 * container referenced by handle `machine_code`.
 *
 * @param machine_code Handle to get the length of.
 * @return The length, in bytes, of the `machine_code`.
 */
size_t machine_code_length(machine_code_t machine_code)
{
    if (machine_code == NULL)
    {
        return 0;
    }
    return machine_code->length;
}

/**
 * Append a byte to the machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param byte A byte to append to the machine code.
 */
void machine_code_append(machine_code_t machine_code, uint8_t byte)
{
    if (machine_code->length == machine_code->allocated_length)
    {
        machine_code_grow(machine_code);
    }
    machine_code->binary[machine_code->length++] = byte;
}

/**
 * Append bytes to the end of machine code.
 *
 * @param machine_code Handle to the machine code to append.
 * @param length Number of bytes to append to the machine code.
 * @param bytes Array of bytes to append
 */
void machine_code_append_array(machine_code_t machine_code, size_t length, uint8_t bytes[])
{
    size_t i = 0;
    for (i = 0; i < length; i++)
    {
        machine_code_append(machine_code, bytes[i]);
    }
}

/**
 * Insert machine code into a program segment.
 *
 * @param machine_code Handle to the machine code to insert.
 * @param address Address to write the code into.
 */
void machine_code_insert(machine_code_t machine_code, intptr_t address)
{

}




