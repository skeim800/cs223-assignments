/***************************************************
 * exclaim.c
 * Author: Sarah Keim 
 * Converts word to exclamation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void convert_to_exclaim(char* input){
  time_t t;
  srand((unsigned) time(&t));  
  char character_vals[5] = {'@', '!', '#', '*'};
  for(int i = 0; i < (strlen(input) + 1); i++){ 
    if (97 <= input[i] && input[i] <= 122){
      int rand_char_index = (rand() % 4); 
      input[i] = character_vals[rand_char_index];
    }
  }
} 


int main() {
  printf("Enter a word: ");
  char input[32];
  scanf("%s", input);
  convert_to_exclaim(input);
  printf("OMG! %s\n", input);
  return 0;
}
