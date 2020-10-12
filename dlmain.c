#include <dlfcn.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_IDENT "dpatch"

int main(int argc, char** argv)
{
    char* target = argv[1];
    void* target_handle = NULL;
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
    }
    closelog();
    exit(EXIT_SUCCESS);
}