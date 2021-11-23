#include <stdio.h>
#include <lib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

int main(int argc, char* argv) {
    int i;
    for(i = 0; i < 40000; i++)
        sqrt(i);
    printf("\tPID: %d\n", getpid()); 
    return 0;
}
