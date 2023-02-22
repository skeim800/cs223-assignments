/*----------------------------------------------
 * Author: Sarah Keim 
 * Date: February 22, 2023
 * Generates a crossword based on two words input by the user
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	char word1[20];
	char word2[20];
	char** crossword;
		
	if (argc < 3){
  		printf("Usage: %s\n", argv[0]);
		exit(0);
	}

	strcpy(word1, argv[1]);
	strcpy(word2, argv[2]);
	int word1CommonIndex = -1;
	int word2CommonIndex = -1;
	for(int i = 0; i < strlen(word1); i++){
		for(int j = 0; j < strlen(word2); j++){
			if(word1[i] == word2[j]){
				word1CommonIndex = i;
				word2CommonIndex = j;
				break;
			}
		if (word1CommonIndex > -1 && word2CommonIndex > -1){
			break;
		}
		}
	}
	//printf("%c %c", word1[word1CommonIndex], word2[word2CommonIndex]);
	if(word1CommonIndex == -1 || word2CommonIndex == -1){
		printf("No common letter!\n");
		return 0;
	}

	crossword = malloc(sizeof(char*) * strlen(word1));
	for(int i = 0; i <  strlen(word1); i++){
		crossword[i] = malloc(sizeof(char) * strlen(word2));
		for(int j = 0; j < strlen(word2); j++){
			if(i == word1CommonIndex){
				crossword[i][j] = word2[j];
			}
			if(j == word2CommonIndex){
				crossword[i][j] = word1[i];
			}
			if(i != word1CommonIndex && j != word2CommonIndex){
				crossword[i][j] = '.';
			}	
		}
	}

	for(int i = 0; i < strlen(word1); i++){
		for(int j = 0; j < strlen(word2); j++){
			printf("%c ", crossword[i][j]);
		}
		printf("\n");
	}
	
	for(int i = 0; i < strlen(word1); i++){
		free(crossword[i]);
	}
	free(crossword);	
	
	return 0;
}
