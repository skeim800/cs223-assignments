/***
 * Author: Sarah Keim
 * Date: April 12, 2023
 * Description: generate a buddhabrot image using 4 threads
 ***/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"

int size = 480;
float xmin = -2.5;
float xmax = 1.0;
float ymin = -1.12;
float ymax = 1.12;
int maxIterations = 1000;
int numProcesses = 4;
int maxCount = 0;
struct ppm_pixel** image;
int** counts;
int** membership;
pthread_mutex_t mutex;
pthread_barrier_t barrier;

struct thread_data{
  struct ppm_pixel color;
  int colX, colY, rowX, rowY;
  int iter;
  float xfrac, yfrac, x0, y0, x, y, xtemp;
};

void* start(void* quadrant_data){
  struct thread_data* data = (struct thread_data*) quadrant_data;
  printf("%ld) Sub-image block: cols (%d, %d) to rows (%d, %d)\n", pthread_self(), data->colX, data->colY, data->rowX, data->rowY);
  

  //step 1
  for(int i = data->rowX; i < data->rowY; i++){
    for(int j = data->colX; j < data->colY; j++){
      data->xfrac = (float) j / size;
      data->yfrac = (float) i / size;
      data->x0 = xmin + data->xfrac * (xmax - xmin);
      data->y0 = ymin + data->yfrac * (ymax - ymin);

      data->x = 0;
      data->y = 0;
      data->iter = 0;
      while(data->iter < maxIterations && data->x*data->x + data->y*data->y < 2*2){
        data->xtemp = data->x*data->x - data->y*data->y + data->x0;
        data->y = 2*data->x*data->y + data->y0;
        data->x = data->xtemp;
        data->iter = data->iter + 1;
      }

      if(data->iter < maxIterations){
        membership[i][j] = 0;
      }
      else{
        membership[i][j] = 1;
      }
    }
  }

  //step 2
  int yrow, xcol;
  for(int i = data->rowX; i < data->rowY; i++){
    for(int j = data->colX; j < data->colY; j++){
      if(membership[i][j] == 1){
        continue;
      }
      
      data->xfrac = (float) j / size;
      data->yfrac = (float) i / size;
      data->x0 = xmin + data->xfrac * (xmax - xmin);
      data->y0 = ymin + data->yfrac * (ymax - ymin);
        
      data->x = 0;
      data->y = 0;
      while((data->x*data->x + data->y*data->y) < 2*2){
        data->xtemp = data->x*data->x - data->y*data->y + data->x0;
        data->y = 2*data->x*data->y + data->y0;
        data->x = data->xtemp;
          
        yrow = round(size * (data->y-ymin)/(ymax-ymin));
        xcol = round(size * (data->x-xmin)/(xmax - xmin));
        if(yrow < 0 || yrow >= size){
          continue;
        }
        if(xcol < 0 || xcol >= size){
          continue;
        }
        
        
        counts[yrow][xcol] = counts[yrow][xcol] + 1;
        pthread_mutex_lock(&mutex);
        maxCount = maxCount + 1;
        pthread_mutex_unlock(&mutex);
      }
    }
  }

  pthread_barrier_wait(&barrier);

  //step3
  float gamma = 0.681;
  float factor = 1.0/gamma;
  for(int i = data->rowX; i < data->rowY; i++){
    for(int j = data->colX; j < data->colY; j++){
      float value = 0; 
      if(counts[i][j] > 0){
        value = (log(counts[i][j])) / (log(maxCount));
        value = pow(value, factor);
      }
      
      data->color.red = value * 255;
      data->color.green = value * 255;
      data->color.blue = value * 255;
       
      
      image[i][j] = data->color;
    }
  }

  return (void*) NULL;
}

int main(int argc, char* argv[]) {
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
  printf("Generating buddhabrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  image = malloc(sizeof(struct ppm_pixel*) * size);
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

  counts = malloc(sizeof(int*) * size);
  if(counts == NULL){
    printf("Memory not allocated.");
    exit(1);
   }
  for(int i = 0; i < size; i++){
    counts[i] = malloc(sizeof(int) * size);
    if(counts[i] == NULL){
      printf("Memory not allocated.");
      exit(1);
    }
  }

  membership = malloc(sizeof(int*) * size);
  if(membership == NULL){
    printf("Memory not allocated.");
    exit(1);
  }
  for(int i = 0; i < size; i++){
    membership[i] = malloc(sizeof(int) * size);
    if(membership[i] == NULL){
      printf("Memory not allocated.");
      exit(1);
    }
  }

  pthread_t threads[4];
  struct thread_data data[4];
  double timer;
  struct timeval tstart, tend;
  gettimeofday(&tstart, NULL);
  pthread_mutex_init(&mutex, NULL);
  pthread_barrier_init(&barrier, NULL, 4);
  for(int i = 0; i < numProcesses; i++){
    if(i == 0){
      data[i].colX = 0;
      data[i].colY = size/2;
      data[i].rowX = 0;
      data[i].rowY = size/2;
    }
    else if(i == 1){
      data[i].colX = size/2;
      data[i].colY = size;
      data[i].rowX = 0;
      data[i].rowY = size/2;
    }
    else if(i == 2){
      data[i].colX = 0;
      data[i].colY = size/2;
      data[i].rowX = size/2;
      data[i].rowY = size;
    }
    else if(i == 3){
      data[i].colX = size/2;
      data[i].colY = size;
      data[i].rowX = size/2;
      data[i].rowY = size;
    }
    pthread_create(&threads[i], NULL, start, (void *) &data[i]);
  }

  for(int i = 0; i < numProcesses; i++){
    pthread_join(threads[i], NULL);
    printf("Thread %ld) finished\n", pthread_self());
  }
  pthread_mutex_destroy(&mutex);
  pthread_barrier_destroy(&barrier);
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;

  char filename[200];
  sprintf(filename, "buddhabrot-%d-%ld.ppm", size, time(NULL));
  write_ppm_2d(filename, image, size, size);

  printf("Completed buddhabrot set (%dx%d) in %f seconds\n", size, size, timer);
  printf("Writing file: %s\n", filename);

  for(int i = 0; i < size; i++){
    free(image[i]);
    free(membership[i]);
    free(counts[i]);
  }

  free(image);
  free(membership);
  free(counts);

  return 0;
}
