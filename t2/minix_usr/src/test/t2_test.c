#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>


int setprocgroup(int pid, int group) {
    if (group == 0 || group == 1) {
        message m;
        m.m1_i1 = pid;
        m.m1_i2 = group;
        return _syscall(MM, SETPROCGROUP, &m);
    }
    return EINVAL;
}

int setgroupratio(int ratio) {
    if (ratio > 0 && ratio < 100) {
        message m;
        m.m1_i1 = ratio;
        return _syscall(MM, SETGROUPRATIO, &m);
    }
    return EINVAL;
}

int main(int argc, char* argv) {
    clock_t start, end;
    double dtime;
    int i, a, b;
    a = setprocgroup(getpid(), 1);
    b = setgroupratio(25);
    printf("%d", a);
    start = clock();
    for(i = 0; i < 3000; i++)
        sqrt(i);
    end = clock();
    dtime = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("\tPID: %d\nTime: %f s\n", getpid(), dtime); 
    return 0;
}
