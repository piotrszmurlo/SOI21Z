#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <lib.h>
#include <time.h>

int getprocgroup(int pid) {
   message m;
   m.m1_i1 = pid;
   return _syscall(MM, GETPROCGROUP, &m);
}

int setabratio(int time) {
   message m;
   m.m1_i1 = time;
   return _syscall(MM, SETABRATIO, &m);
}

int main(int argc, char* argv[]) {
    int time, result;
    pid_t pid = getpid();
    int group = getprocgroup(pid);
    if(argc != 2) {
       printf("Brak argumentu\n");
       return 0;
    } 
    else {
       time = atoi(argv[1]);
       printf("Kwant czasu dla procesu z grupy A: %d\nOdpowiednio dla procesu z grupy B: %d\n", time, 100-time);
       result = setabratio(time);
       printf("Aktualny proces (pid: %d) nalezy do grupy %d.\n", pid, group);
 }   
}