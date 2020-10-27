#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("I was loaded at 0x%p.\n", &main);
    exit(EXIT_SUCCESS);
}
