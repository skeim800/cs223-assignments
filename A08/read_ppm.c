/*----------------------------------------------
 * Author: Sarah Keim 
 * Date: February 22, 2023
 * Reads pixel values in from ppm file and puts them in a 2D array
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

// Choose *one* to implement (do not remove the other one!)

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {

	return NULL;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {
  	FILE* fp;
	char line[200];
	struct ppm_pixel** data;

	printf("%s\n", filename);	

	fp = fopen(filename, "rb");
	if(fp == NULL){
		printf("Error: unable to open file %s.\n", filename);
		exit(1);
	}


	//loop for header, gets width and height
	for(int i = 0; i < 4; i++){
		fgets(line, 200, fp); 
		if(i == 2){
			*w = atoi(&line[0]);
			*h = atoi(&line[2]);
		}
	}

	//creates 2D array for pixel data, then reads file and assigns to values in array
	data = malloc(sizeof(struct ppm_pixel*) * *h);
	if(data == NULL){
		printf("Memory not allocated.");
		exit(1);
	}
	for(int j = 0; j < *h; j++){
		data[j] = malloc(sizeof(struct ppm_pixel) * *w);
		if(data[j] == NULL){
			printf("Memory not allocated.");
			exit(1);
		}
	}	
	for(int j = 0; j < *h; j++){
		for(int k = 0; k < *w; k++){
			fread(&data[j][k].red, sizeof(unsigned char), 1, fp);
			fread(&data[j][k].green, sizeof(unsigned char), 1, fp);
			fread(&data[j][k].blue, sizeof(unsigned char), 1, fp);
		}
	}
	
	fclose(fp);
	return data;
}
