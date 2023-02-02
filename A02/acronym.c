/***************************************************
 * acronym.c
 * Author: Sarah Keim
 * Creates an acronym from a phrase
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void make_acronym(char* input, char* acronym){
  char* split = strtok(input, " ");
  int count = 0;
  //code for tokenization came from Professor Aline's lecture slides
  while (split != NULL){
    if (65 <= split[0] && split[0] <= 90){
      acronym[count] = split[0];
      count++;
    }
    split = strtok(NULL, " ");
  }
}

int main() {
  printf("Enter a phrase: ");
  char phrase[1024] = "";
  scanf("%[^\n]%*c", phrase);
  char acronym[128] = "";
  make_acronym(phrase, acronym);
  printf("Your acronym is %s\n", acronym);
  return 0;
}
