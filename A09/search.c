/***
 * Author: Sarah Keim
 * Date: April 4, 2023
 * Description: searches and returns the index for an integer from a file of integers using N threads
 ***/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int num_to_find;
static int index_of_num = -1;
static int thread_found;

struct thread_data{
	int start_index;
	int end_index;
	int* array;
	int id;
};

void* thread_search(void* user_data){
	struct thread_data* data = (struct thread_data*) user_data;
	for(int i = data->start_index; i < data->end_index; i++){
		//printf("data[%d]: %d\n",i,  data->array[i]);
		if(data->array[i] == num_to_find){
			index_of_num = i;
			thread_found = data->id;
		}
	}
	return (void*) NULL;
}	

int main(int argc, char** argv) {
	int num_ints = 0;
      	int* numbers;
	int num_threads = 0;	
	
	if (argc < 2) {
    		printf("usage: %s <NumThreads>\n", argv[0]);
		exit(0);
  	}

	num_threads = atoi(argv[1]);
	
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

	pthread_t* threads = malloc(sizeof(pthread_t)*num_threads);
	struct thread_data* data = malloc(sizeof(struct thread_data)*num_threads);
	int subsize = num_ints/num_threads;
	for(int i = 0; i < num_threads; i++){
		data[i].array = numbers;
		data[i].start_index = subsize*i;
		data[i].end_index = subsize*(i+1);
		data[i].id = i + 1;
		pthread_create(&threads[i], NULL, thread_search, (void*) &data[i]);
	}

	for(int i = 1; i < num_threads+1; i++){
		pthread_join(threads[i], NULL);
		//printf("%d %ld %ld\n", i, threads[i], thread_ids[i]);
	}
	
	if(index_of_num >= 0){
		printf("Thread %d found %d at index %d\n", thread_found, num_to_find, index_of_num);
	}
	else{
		printf("Not found!\n");
	}

	free(numbers);
	free(threads);
	free(data);

  	return 0;
}
