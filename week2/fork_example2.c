#include <stdio.h>
#include <unistd.h>

int main()
{
    if (fork() == 0)
    {
        // child process because return value zero
        printf("Hello from Child!\n");
    }
    else
    {
        // parent process because return value non-zero
        printf("Hello from Parent!\n");
    }
    return 0;
}