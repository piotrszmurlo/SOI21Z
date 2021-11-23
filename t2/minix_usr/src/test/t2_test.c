#include <stdio.h>
#include <lib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

void child_action(void){
    clock_t start, end;
    double dtime;
    int i;
    start = clock();
    for(i = 0; i < 20000 ; i++)
        sqrt(i);
    end = clock();
    dtime = ((double)(end - start))/CLOCKS_PER_SEC;
    printf("\tPID: %d\nTime: %f s\n", getpid(), dtime); 
}

int main(int argc, char* argv) {

int j;
int s;
for(j = 0; j<3;j++)
{
	s = fork();
	if(s)
	{

}
else
{
	child_action();
	exit(0);
}

    return 0;
}
