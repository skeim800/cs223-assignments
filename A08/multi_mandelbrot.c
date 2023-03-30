/***
 * Author: Sarah Keim
 * Date: March 23, 2023
 * Description: splits the generation of a mandelbrot image into 4 processes 
 ***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char* argv[]) {
  	int size = 480;
  	float xmin = -2.0;
  	float xmax = 0.47;
  	float ymin = -1.12;
  	float ymax = 1.12;
  	int maxIterations = 1000;
  	int numProcesses = 4;

  	int opt;
  	while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
    		switch (opt) {
      			case 's': size = atoi(optarg); break;
      			case 'l': xmin = atof(optarg); break;
      			case 'r': xmax = atof(optarg); break;
      			case 't': ymax = atof(optarg); break;
      			case 'b': ymin = atof(optarg); break;
      			case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        		"-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]); break;
    		}
  	}
  	printf("Generating mandelbrot with size %dx%d\n", size, size);
  	printf("  Num processes = %d\n", numProcesses);
  	printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
	printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);
	
  	// creates pallete
	srand(time(0));
	int shmid1 = shmget(IPC_PRIVATE, sizeof(int) * maxIterations, 0644 | IPC_CREAT);
	if(shmid1 == -1){
		perror("Error: cannot initialize shared memory\n");
		exit(1);
	}

	struct ppm_pixel* palette = shmat(shmid1, NULL, 0);
	if(palette == (void*) -1){
		perror("Error: cannot intialize shared memory\n");
		exit(1);
	}

	int count = 0;
	for(int i = 0; i < size; i++){
		palette[i].red = rand() % 255;
		palette[i].green = rand() % 255;
		palette[i].blue = rand() % 255;
		count += 1;
	}
	
	//creates memory for image
	int shmid2;
	shmid2 = shmget(IPC_PRIVATE, sizeof(struct ppm_pixel*) * size, 0644 | IPC_CREAT);
	if(shmid2 == -1){ 
		perror("Error: cannot initialize shared memory\n");
		exit(1);
	}

	struct ppm_pixel** buffer = shmat(shmid2, NULL, 0);
	if(buffer == (void*) -1){
		perror("Error: cannot access shared memory\n");
		exit(1);
	}				
	
	int* shmid3 = malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++){
		shmid3[i] = shmget(IPC_PRIVATE, sizeof(struct ppm_pixel) * size, 0644 | IPC_CREAT);
		buffer[i] = shmat(shmid3[i], NULL, 0);
		if (buffer[i] == (void*) -1){
			perror("Error: cannot access shared memory\n");
			exit(1);
		}
	}

	//generates image in 4 pieces
	double timer;
	struct timeval tstart, tend;
	float xfrac, yfrac, x0, y0, x, y, xtemp;
	int iter;
	int colX, colY, rowX, rowY;
	gettimeofday(&tstart, NULL);
	for(int i = 0; i < 4; i++){
		if(i == 0){
			colX = 0;
			colY = size/2;
			rowX = 0;
			rowY = size/2;
		}
		else if(i == 1){
			colX = size/2;
			colY = size;
			rowX = 0;
			rowY = size/2;
		}
		else if (i == 2){
			colX = 0;
			colY = size/2;
			rowX = size/2;
			rowY = size;
		}
		else if (i == 3){
			colX = size/2;
			colY = size;
			rowX = size/2;
			rowY = size;
		}
		int pid = fork();
		if(pid == 0){
			printf("%d) Sub-image block: cols (%d, %d) to rows (%d, %d)\n", getpid(), colX, colY, rowX, rowY);
			for(int j = rowX; j < rowY; j++){
				for(int k = colX; k < colY; k++){
					xfrac = (float) j / size;
					yfrac = (float) k / size;
					x0 = xmin + xfrac * (xmax - xmin);
					y0 = ymin + yfrac * (ymax - ymin);

					x = 0;
					y = 0;
					iter = 0;
					while(iter < maxIterations && x*x + y*y < 2*2){
						xtemp = x*x - y*y + x0;
						y = 2*x*y + y0;
						x = xtemp;
						iter = iter + 1;
					}

					if (iter < maxIterations){
						buffer[j][k] = palette[iter];
					}
					else{
						buffer[j][k].red = 0;
						buffer[j][k].green = 0;
						buffer[j][k].blue = 0;
					}
				}
			}
			exit(0);
		}
		else{
			printf("Launched child process: %d\n", pid);
		}
	}

	for(int i = 0; i < 4; i++){
		int status;
		int pid = wait(&status);
		printf("Child process complete: %d\n", pid);
	}
	gettimeofday(&tend, NULL);
	timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
	
	char filename[200];
	sprintf(filename, "mandelbrot-%d-%ld.ppm", size, time(NULL));
	write_ppm_2d(filename, buffer, size, size);
	
	printf("Computed mandelbrot set(%dx%d) in %f seconds\n", size, size, timer);
	printf("Writing file: %s\n", filename);

	for(int i = 0; i < size; i++){
		if(shmdt(buffer[i]) == -1){
			perror("Error: cannot detach from shared memory\n");
			exit(1);
		}
		if(shmctl(shmid3[i], IPC_RMID, 0) == -1){
			perror("Error: cannot remove shared memory\n");
			exit(1);
		}
	}
	
	if(shmdt(buffer) == -1){
		perror("Error: cannot detach from shared memory\n");
		exit(1);
	}
  
  	if(shmdt(palette) == -1){
    		perror("Error: cannot detach from shared memory\n");
    		exit(1);
  	}

	if(shmctl(shmid1, IPC_RMID, 0) == -1){
		perror("Error: cannot remove shared memory\n");
		exit(1);
	}

  	if(shmctl(shmid2, IPC_RMID, 0) == -1){
    		perror("Error: cannot remove shared memory\n");
    		exit(1);
  	}

  	return 0;
}
