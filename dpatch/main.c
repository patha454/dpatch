#include <dlfcn.h>
#include <unistd.h>
#include <syslog.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

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
 * Insert an x64 `UD0` (undefined operation) opcode into the program.
 *
 * Inserting and then executing the undefined operation should cause a runtime
 * fault.
 *
 * @param addr  Address to start the opcode ad.
 */ 
void insert_undefied_op(intptr_t addr)
{
    /* Assume we're compiling for x64, so this should already be little-endian,
     * as an x64 CPU expects its instructions to be.
     */
    const uint16_t x64_ud0 = 0x0b0f;
    uint16_t* const target = (uint16_t*) addr;
    *target = x64_ud0;
    return;
}

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
 * Attempt to modify the program code at a selected address.
 *
 * `attempt_metamorphosis` will attempt to insert an undefined opcode at the
 * target address to prove we can modify the running binary.
 *
 * @param addr  The base address to attempt to modify.
 * @return      The success, or otherwise, of the operation.
 */
dpatch_status attempt_metamorphosis(intptr_t addr)
{
    dpatch_status status = DPATCH_STATUS_OK;
    status = mprotect_round(addr, 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    if (status != DPATCH_STATUS_OK)
    {
        return status;
    }
    insert_undefied_op(addr);
    status = mprotect_round(addr, 2, PROT_READ | PROT_EXEC);
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
    attempt_metamorphosis((intptr_t) target_start);
    target_start();
    closelog();
    exit(EXIT_SUCCESS);
}
