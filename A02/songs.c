/***************************************************
 * songs.c
 * Author: Sarah Keim 
 * Allows user to edit list of songs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct song{
  char artist[32];
  char title[32];
  int mins;
  int secs;
  float danceability;
};

void print_song_table(struct song song_list[]){
  printf("Welcome to Steven Struct's Song List.\n\n");
  for(int i = 0; i < 3; i++){
    printf("%d) %-20s artist: %-20s duration: %d:%d\tdanceability: %.2f\n", 
      i, song_list[i].title, song_list[i].artist, song_list[i].mins, 
      song_list[i].secs, song_list[i].danceability);
  }
  printf("\n=======================\n\n");
}

int main() {
  struct song song_list[3];
  strcpy(song_list[0].artist, "Tears for Fears");
  strcpy(song_list[0].title, "Shout");
  song_list[0].mins = 4;
  song_list[0].secs = 11;
  song_list[0].danceability = 0.5;
  strcpy(song_list[1].artist, "Harry Styles");
  strcpy(song_list[1].title,"As it was");
  song_list[1].mins = 2;
  song_list[1].secs = 40;
  song_list[1].danceability = 0.7;
  strcpy(song_list[2].artist,"Pink Floyd");
  strcpy(song_list[2].title, "Wish you were here");
  song_list[2].mins = 5;
  song_list[2].secs = 34;
  song_list[2].danceability = 0.3;
  print_song_table(song_list);
  printf("Enter a song id to edit [0,1,2]: ");
  int id = 0;
  scanf("%d", &id);
  while(id < 0 || id > 2){
    printf("I'm sorry, please enter a valid song id.\n");
    printf("Enter a song id to edit [0,1,2]: ");
    scanf("%d", &id);
  }
  printf("Which attribute do you wish to edit? [artist, title, duration, danceability]: ");
  char attribute[32] = "";
  scanf("%s", attribute);
  if(strcmp(attribute, "artist") == 0){
    printf("Enter an artist: ");
    char value[32] = ""; 
    scanf("\n");
    scanf("%[^\n]%*c", strcpy(song_list[id].artist, value));
  }
  else if(strcmp(attribute, "title") == 0){
    printf("Enter a title: ");
    char value[32] = "";
    scanf("\n");
    scanf("%[^\n]%*c", strcpy(song_list[id].title, value));
  }
  else if(strcmp(attribute, "duration") == 0){
    printf("Enter a duration (minutes): ");
    scanf("\n");
    scanf("%d", &song_list[id].mins);
    printf("Enter a duration (seconds): ");
    scanf("\n");
    scanf("%d", &song_list[id].secs);
  }
  else if(strcmp(attribute, "danceability") == 0){
    printf("Enter danceability: ");
    scanf("\n");
    scanf("%f", &song_list[id].danceability);
  }
  else{
    printf("Invalid attribute.\n");
  }
  print_song_table(song_list);
  return 0;
}
