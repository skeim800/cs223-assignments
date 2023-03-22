/***
 * Author: Sarah Keim
 * Date: March 22, 2023
 * Description: program that outputs a PPM image of the the mandelbrot set
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char* argv[]) {
 	int size = 480;
  	float xmin = -2.0;
  	float xmax = 0.47;
  	float ymin = -1.12;
  	float ymax = 1.12;
  	int maxIterations = 1000;

	int opt;
  	while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
    		switch (opt) {
      			case 's': size = atoi(optarg); break;
      			case 'l': xmin = atof(optarg); break;
      			case 'r': xmax = atof(optarg); break;
      			case 't': ymax = atof(optarg); break;
      			case 'b': ymin = atof(optarg); break;
      			case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax>\n", argv[0]); break;
    		}
  	}
  	printf("Generating mandelbrot with size %dx%d\n", size, size);
  	printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  	printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);
	
	//creates the palette
	srand(time(0));
  	struct ppm_pixel palette[1000];
	int count = 0;
	for(int i = 0; i < maxIterations; i++){
		palette[i].red = rand() % 255;
		palette[i].green = rand() % 255;
		palette[i].blue = rand() % 255;
		count += 1;
	}
	
	//creates image
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
	
	double timer;
	struct timeval tstart, tend;
	float xfrac;
	float yfrac;
	float x0;
	float y0;
	float x;
	float y;
	int iter;
	float xtemp;
	gettimeofday(&tstart, NULL);
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			xfrac = (float) i / size;
			yfrac = (float) j / size;
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

			if(iter < maxIterations){
				image[i][j] = palette[iter];
			}
			else{
				image[i][j].red = 0;
				image[i][j].green = 0;
				image[i][j].blue = 0;
			}
		}
	}
	gettimeofday(&tend, NULL);
	timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
	
	char filename[200];
	sprintf(filename, "mandelbrot-%d-%ld.ppm", size, time(NULL));
	write_ppm_2d(filename, image, size, size);
		
	printf("Computed mandelbrot set(%dx%d) in %f seconds\n", size, size, timer);
	printf("Writing file: %s\n", filename);
	
	for(int i = 0; i < size; i++){
		free(image[i]);
	}
	free(image);

	return 0;
}

