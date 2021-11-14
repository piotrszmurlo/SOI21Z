#include <stdio.h>
#include <lib.h>
#include <stdlib.h>

int getprocnr(int proc_id)
{
	message msg;
	msg.m1_i1 = proc_id;
	return _syscall(0, 78, &msg);
}

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		int proc_id;
		int i;
		int result;
		proc_id = atoi(argv[1]);
		for (i = proc_id; i <= proc_id + 10; i++)
		{
			result = getprocnr(i);
			if (result == -1)
			{
				printf("error: process with id %d not found. errno: %d\n", i, errno);
			}
			else
			{
				printf("index of process (id=%d): %d\n", i, result);
			}
		}
	}
	else
	{
		printf("Usage: './lab1_test [pid]'\n");
	}
	return 0;
}
