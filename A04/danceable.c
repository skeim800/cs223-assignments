/*----------------------------------------------
 * Author: Sarah Keim
 * Date: February 15, 2023
 * Tells user what song is most danceable from a list of songs
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node{
	char title[128];
	char artist[128];
	int minutes;
	int seconds;
	float danceability;
	float energy;
	float tempo;
	float valence;
	struct node* next;
};	

struct node* insert_front(struct node song, struct node* head){
	struct node* n = malloc(sizeof(struct node));
	if (n == NULL){
		printf("ERROR: Out of space!\n");
		exit(1);
	}
	strcpy(n->title, song.title);
	strcpy(n->artist, song.artist);
	n->minutes = song.minutes;
	n->seconds = song.seconds;
	n->danceability = song.danceability;
	n->energy = song.energy;
	n->tempo = song.tempo;
	n->valence = song.valence;
	n->next = head;
	return n;
}

void delete_node(struct node song, struct node** head){
	struct node* placeholder = *head;
	struct node* previous = placeholder;
	if(placeholder != NULL && strcmp(placeholder->title, song.title) == 0){
		*head = (*head)->next;
		free(placeholder);
		placeholder = NULL;
	}
	else{
		while(placeholder != NULL && strcmp(placeholder->title, song.title) != 0){
			previous = placeholder;
			placeholder = placeholder->next;
		}
		if(placeholder == NULL){
			return;
		}
		previous->next = placeholder->next;
		free(placeholder);
	}
}

void clear_list(struct node** head){
	struct node* n = *head;
	while(n != NULL){
		*head = (*head)->next;
		free(n);
		n = *head;
	}	
}

void print_list(struct node* list, int* i){
	*i = 0;
	if(list == NULL){
		printf("Dataset contains 0 songs.\n\n");
	}
	for(struct node* n = list; n != NULL; n = n->next){
		printf("%d) %-25sartist: %-20s(%d:%02ds)\tD: %.3f\tE: %.3f\tT: %.3f\tV: %.3f\n"
			,*i, n->title, n->artist, n->minutes, n->seconds, n->danceability, n->energy, n->tempo, n->valence);
		*i = *i + 1;
	}
}

struct node most_danceable(struct node** head){
	float most_dance_val = 0.0;
	struct node most_dance;
	struct node* n = *head;
	while(n != NULL){
		if(n->danceability > most_dance_val){
			most_dance_val = n->danceability;
			most_dance = *n;
		}
		n = n->next;
	}
	return(most_dance);
}

int main() {
  	FILE* infile;
	char line[1024];
	char* token;
	struct node* song_list = NULL;
	struct node song;
	int num_songs = 0;

	infile = fopen("songlist.csv", "r");
	if(infile == NULL){
		printf("Error: unable to open file %s\n", "songlist.csv");
		exit(1);
	}

	fgets(line, 1024, infile); //ignores the first line with the headers
	while(fgets(line, 1024, infile)){
		token = strtok(line, ",");
		int i = 0;
		while (token) {
			if (i == 0){
				strcpy(song.title, token);
			}
			if(i == 1){
				strcpy(song.artist, token);
			}
			else if (i == 2){
				song.minutes = (atoi(token) / 1000) / 60;
				song.seconds = (atoi(token) / 1000) % 60;
			}
			else if (i == 3){
				song.danceability = atof(token);
			}
			else if (i == 4){
				song.energy = atof(token);
			}
			else if (i == 5){
				song.tempo = atof(token);
			}
			else if (i == 6){
				song.valence = atof(token);
			}
			else if (i ==7){
				break;
			}
			i = i + 1;
			if (i == 6){
				token = strtok(NULL, "\r\n");
			}
			else{
				token = strtok(NULL, ",");
			}
		}
		song_list = insert_front(song, song_list);
	}
	print_list(song_list, &num_songs);
	while(song_list != NULL){
		printf("\nDataset contains %d songs.\n\n=========================\n", num_songs);
		printf("Press 'd' to show the most danceable song (any other key to quit): ");
		char input;
		scanf(" %c", &input);
		if(input == 100){
			struct node most = most_danceable(&song_list);
			printf("\n------------------------------------------- Most Danceable"
				"-------------------------------------------\n%-25s%-15s"
				"(%d:%ds)\tD: %.3f\tE: %.3f\tT: %.3f\tV: %.3f\n\n"
				,most.title, most.artist, most.minutes, most.seconds, 
				most.danceability, most.energy, most.tempo, most.valence);	
			delete_node(most_danceable(&song_list), &song_list);
	       		print_list(song_list, &num_songs);	
		}
		else{
			break;
		}
	}
	
	
	fclose(infile);
	clear_list(&song_list);

	return 0;
}
