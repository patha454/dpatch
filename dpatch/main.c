#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/mman.h>
#include <unistd.h>

#define PROGRAM_IDENT "dpatch"
#define START_SYMBOL "main"

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
    uint8_t bytecode[15];
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

int main(int argc, char** argv)
{
    char* target = argv[1];
    void* target_handle = NULL;
    void (* target_start)(void) = NULL;
    openlog(PROGRAM_IDENT, LOG_PERROR, LOG_USER);
    if (argc < 2)
    {
        syslog(LOG_ERR, "A target program must be provided as an argument.");
        closelog();
        exit(EXIT_FAILURE);
    }
    target_handle = dlopen(target, RTLD_LAZY);
    if (target_handle == NULL)
    {
        syslog(LOG_ERR, "dlopen could not load the target '%s.'", target);
        exit(EXIT_FAILURE);
    }
    /*
     * Casting an object pointer to a function pointer is not strictly valid
     * ANSI C, because some machines have diffrent data and instruction pointer
     * widths. Diffrent data and instruction widths is largely an archaic detail
     * and the design of `dlfcn` forces us to make the cast. Disable pedantic 
     * errors for the cast.
     */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    target_start = (void (*)(void)) dlsym(target_handle, START_SYMBOL);
    #pragma GCC diagnostic pop
    if (target_start == NULL)
    {
        syslog(
            LOG_ERR,
            "The '%s:%s' symbol could not be located.",
            target,
            START_SYMBOL
        );
        exit(EXIT_FAILURE);
    }
    insert_op((intptr_t) target_start, X64_UD2);
    target_start();
    closelog();
    exit(EXIT_SUCCESS);
}
