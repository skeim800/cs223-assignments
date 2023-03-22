/*----------------------------------------------
 * Author: Sarah Keim 
 * Date: February 22, 2023
 * Writes pixel data to a ppm file
 ---------------------------------------------*/
#include "write_ppm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// implement *one* (but do not remove the other one)!

void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {

}

void write_ppm_2d(const char* filename, struct ppm_pixel** pixels, int w, int h) {
	FILE* fp;
	
	fp = fopen(filename, "wb");
	if(fp == NULL){
		printf("Error: unable to open file %s.\n", filename);
		exit(1);
	}

	//writes header to file
	fprintf(fp, "P6\n");
	fprintf(fp, "# Created by GIMP version 2.10.24 PNM plug-in\n");
	fprintf(fp, "%d %d\n", w, h);
	fprintf(fp, "255\n");

	//writes the pixel data to the file
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			fwrite(&pixels[i][j].red, sizeof(unsigned char), 1, fp);
			fwrite(&pixels[i][j].green, sizeof(unsigned char), 1, fp);
			fwrite(&pixels[i][j].blue, sizeof(unsigned char), 1, fp);
		}
	}

	fclose(fp);
}
