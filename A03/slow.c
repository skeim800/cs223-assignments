/*----------------------------------------------
 * Author:Sarah Keim
 * Date:February 8, 2023
 * Takes user input and returns input with pauses represented as "."
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  int pause_len = 0;
  char* input;

  printf("Pause length: ");
  scanf("%d", &pause_len);
  printf("Text: ");
  input = (char*)malloc(sizeof(char)*32);
  scanf("%s", input);

  for(int i = 0; i < (strlen(input)); i++){
    printf("%c", input[i]);
    for(int j = 0; j < pause_len; j++){
      printf("%c", '.');
    }
  }
  printf("\n");

  free(input);
  return 0;
}

