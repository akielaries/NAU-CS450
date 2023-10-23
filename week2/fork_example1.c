#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Before fork\n");
    fork();
    printf("After 1st fork\n");
    fork();
    printf("After 2nd fork\n");
    return 0;
}
