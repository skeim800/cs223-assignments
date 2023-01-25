/***************************************************
 * mathgame.c
 * Author: Sarah Keim 
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
 printf("Welcome to Math Game!\n");
 printf("How many rounds do you want to play? ");
 int rounds = 0;
 scanf("%d", &rounds);
 int num_correct = 0;
 time_t t;
 srand((unsigned) time(&t));

 for (int i=0;i<rounds;i++){
  int num_one = rand() % 10;
  int num_two = rand() % 10;
  printf("%d + %d = ? ", num_one, num_two);
  int response;
  scanf("%d", &response);
  if (response == num_one + num_two){
   num_correct += 1;
   printf("Correct!\n");
  }
  else {
   printf("Incorrect :(\n");
  }
 }

 printf("You answered %d/%d correctly.\n", num_correct, rounds);

 return 0;
}
 
