/*----------------------------------------------
 * Author: Sarah Keim
 * Date: February 22, 2023
 * Takes pixel data and writes to new ppm file
 ---------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
	int w, h;
  	struct ppm_pixel** pixels = read_ppm_2d("feep-raw.ppm", &w, &h);

  	// test writing the file to test.ppm, reload it, and print the contents
  	write_ppm_2d("test.ppm", pixels, w, h);
  	
	//checks that write was completed correctly by rerunning read_ppm
	int w2, h2;
  	struct ppm_pixel** pixels_test = read_ppm_2d("test.ppm", &w2, &h2);
  	for(int i = 0; i < h2; i++){
		for(int j = 0; j < w2; j++){
		  	printf("(%u, %u, %u)  ", pixels_test[i][j].red, pixels_test[i][j].green, pixels_test[i][j].blue);
	  	}
	  	printf("\n");
  	}
  
 	for(int i = 0; i < h; i++){
	  	free(pixels[i]);
	  	free(pixels_test[i]);
  	}
		
  	free(pixels);
  	free(pixels_test);
  	return 0;
}
