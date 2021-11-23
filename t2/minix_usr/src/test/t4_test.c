#include <stdio.h>
#include <lib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

int getprocgroup(int pid) {
   message m;
   m.m1_i1 = pid;
   return _syscall(MM, GETPROCGROUP, &m);
}

int main(int argc, char* argv[]) {
    int i;
    pid_t pid;
    for(i = 0; i < 15000; i++)
        sqrt(i);
    pid = getpid();
    printf("PID: %d; grupa: %d\n", pid, getprocgroup(pid)); 
    return 0;
}
