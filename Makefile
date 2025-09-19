# IONASCU George-Razvan 314CA
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build: main.c
	$(CC) $(CFLAGS) -g -o tema2 main.c

run: build
	./tema2

clean:
	rm -f tema2