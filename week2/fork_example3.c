#include <stdio.h>
#include <unistd.h>

int main()
{
	int count = 0;
	int pid;
	pid = fork();

    printf("\npid: %d", pid);

    if (pid != 0) {
        while (count < 10) {
            count++;
		    printf("\nParent pid: %d sleeping for 1 s", pid);
		    sleep(1);
        }		    
    }
    else
    {
        printf("\nChild pid: %d sleeping for 15 s\n", pid); 
        sleep(15);
    }

    printf("\nCount: %d\n", count); 
    return 0;
}
