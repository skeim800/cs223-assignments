/***
 * Author: Sarah Keim
 * Date: April 4, 2023
 * Description: splits generation of a mandelbrot image into 4 threads
 ***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

struct thread_data{
	struct ppm_pixel* colors;
	struct ppm_pixel** image;
	int colX, colY, rowX, rowY;
	int size, maxIterations;
	float xmin, xmax, ymin, ymax;
};

void* create_image(void* quadrant_data){
	struct thread_data* data = (struct thread_data*) quadrant_data;
	printf("%ld) Sub-image block: cols (%d, %d) to rows (%d, %d)\n", pthread_self(), data->colX, data->colY, data->rowX, data->rowY);
	float xfrac, yfrac, x0, y0, x, y, xtemp;
	int iter;
	for(int i = data->rowX; i < data->rowY; i++){
		for(int j = data->colX; j < data->colY; j++){
			xfrac = (float) j / data->size;
			yfrac = (float) i / data->size;
			x0 = data->xmin + xfrac * (data->xmax - data->xmin);
			y0 = data->ymin + yfrac * (data->ymax - data->ymin);

			x = 0;
			y = 0;
			iter = 0;
			while(iter < data->maxIterations && x*x + y*y < 2*2){
				xtemp = x*x - y*y + x0;
				y = 2*x*y + y0;
				x = xtemp;
				iter = iter + 1;
			}

			if (iter < data->maxIterations){
				data->image[i][j] = data->colors[iter];
			}
			else{
				data->image[i][j].red = 0;
				data->image[i][j].green = 0;
				data->image[i][j].blue = 0;
			}
		}
	}
	return (void*) NULL;
}

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

	srand(time(0));
	struct ppm_pixel palette[1000];
	int count = 0;
	for(int i = 0; i < maxIterations; i++){
		palette[i].red = rand() % 255;
		palette[i].green = rand() % 255;
		palette[i].blue = rand() % 255;
		count += 1;
	}

	struct ppm_pixel** image = malloc(sizeof(struct ppm_pixel*) * size);
	if(image == NULL){
		printf("Memory not allocated.");
		exit(1);
	}
	for(int i = 0; i < size; i++){
		image[i] = malloc(sizeof(struct ppm_pixel) * size);
		if(image[i] == NULL){
			printf("Memory not allocated.");
			exit(1);
		}
	}
	
	pthread_t threads[4];
	struct thread_data data[4];
	double timer;
	struct timeval tstart, tend;
	gettimeofday(&tstart, NULL);
	for(int i = 0; i < numProcesses; i++){
		data[i].colors = palette;
		data[i].image = image;
		data[i].size = size;
		data[i].xmin = xmin;
		data[i].xmax = xmax;
		data[i].ymin = ymin;
		data[i].ymax = ymax;
		data[i].maxIterations = maxIterations;
		if (i == 0){
			data[i].colX = 0;
			data[i].colY = size/2;
			data[i].rowX = 0;
			data[i].rowY = size/2;
		}
		else if (i == 1){
			data[i].colX = size/2;
			data[i].colY = size;
			data[i].rowX = 0;
			data[i].rowY = size/2;
		}
		else if (i == 2){
			data[i].colX = 0;
			data[i].colY = size/2;
			data[i].rowX = size/2;
			data[i].rowY = size;
		}
		else if (i == 3){
			data[i].colX = size/2;
			data[i].colY = size;
			data[i].rowX = size/2;
			data[i].rowY = size;
		}
		pthread_create(&threads[i], NULL, create_image, (void *) &data[i]);
	}

	for(int i = 0; i < numProcesses; i++){
		pthread_join(threads[i], NULL);
		printf("Thread %ld) finished\n", pthread_self());
	}
	gettimeofday(&tend, NULL);
	timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;

	char filename[200];
	sprintf(filename, "mandelbrot-%d-%ld.ppm", size, time(NULL));
	write_ppm_2d(filename, image, size, size);

	printf("Completed mandelbrot set(%dx%d) in %f seconds\n", size, size, timer);
	printf("Writing file: %s\n", filename);

	for(int i = 0; i < size; i++){
		free(image[i]);
	}
	free(image);

	return 0;
}
