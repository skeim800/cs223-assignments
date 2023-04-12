/***
 * Author: Sarah Keim
 * Date: March 29, 2023
 * Descriptioin: searches and returns the index for an integer from a file of integers
 ***/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int num_ints = 0;
	int* numbers;
	int num_to_find;
	pid_t pid;

	FILE* fp = fopen("data.bin", "rb");
	if(fp == NULL){
		printf("Error: unable to open file data.bin");
		exit(1);
	}

	fread(&num_ints, sizeof(int), 1, fp);
	numbers = (int*)malloc(num_ints*sizeof(int));
	fread(numbers, sizeof(int), num_ints, fp);

	printf("Enter a value to search: ");
	scanf("%d", &num_to_find);

	pid = fork();

	if(pid){
		for(int i = num_ints/2; i < num_ints; i++){
			if(numbers[i] == num_to_find){
				printf("Child found %d at index %d\n", num_to_find, i);
				exit(0);
			}
		}
		printf("Not found!\n");
	}
	else{
		for(int i = 0; i < num_ints/2; i++){
			if(numbers[i] == num_to_find){
				printf("Parent found %d at index %d\n", num_to_find, i);
				exit(0);
			}
		}
		printf("Not found!\n");
	}

	free(numbers);	
	fclose(fp);	

	return 0;
}
