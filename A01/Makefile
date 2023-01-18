CC=gcc
% :: %.c
	$(CC) -g -Wall -Wvla -Werror -Wno-unused-variable $< -o $@

all: mathgame hello

mathgame : mathgame.c
	gcc mathgame.c -o mathgame

hello : hello.c
	gcc hello.c -o hello

clean :
	rm hello mathgame
