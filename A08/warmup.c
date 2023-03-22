/***
 * Author: Sarah Keim
 * Date: March 22, 2023
 * Description: practice using fork and udnerstanding parent/child processes
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	pid_t pid;

	printf("%d] A0\n", getpid());
	pid = fork();

	if(pid == 0){
		printf("%d] B1\n", getpid());
		pid = fork();

		if(pid == 0){
			printf("%d] C1\n", getpid());
		}
	}
	else{
		printf("%d] B0\n", getpid());
		printf("%d] C0\n", getpid());
	}


	printf("%d] Bye\n", getpid());
	sleep(5);
	return 0;
}
