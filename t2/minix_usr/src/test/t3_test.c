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

int setprocgroup(int pid, int group) {
   message m;
   m.m1_i1 = pid;
   m.m1_i2 = group;
   return _syscall(MM, SETPROCGROUP, &m);
}

int setabratio(int time) {
   message m;
   m.m1_i1 = time;
   return _syscall(MM, SETABRATIO, &m);
}

int main(int argc, char* argv[]) {
    int new_abratio;
    pid_t pid = getpid();
    int group = getprocgroup(pid);
    if(argc != 2) {
       printf("Podaj kwant czasu dla A [1-99]\n");
       return 0;
    } 
    else {
       new_abratio = atoi(argv[1]);
       printf("Kwant czasu procesow grupy A: %d; grupy B: %d;\n", new_abratio, 100-new_abratio);
       setabratio(new_abratio);
       printf("Aktualny proces (pid: %d) przynalezy do grupy %d.\n", pid, getprocgroup(pid));
       setprocgroup(pid, abs(group - 1));
       printf("Zmiana grupy za pomoca wywolania systemowego: (pid: %d) przynalezy do grupy %d.\n", pid, getprocgroup(pid));
 }   
}