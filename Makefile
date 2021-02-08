CC     = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -g
LIBS   = -pthread

DEPS = src/snake/snake.h src/simpcg/simpcg.h src/simpcg/pixel-buffer.h Makefile

.PHONY: all make-dirs clean

all: make-dirs bin/snake-game

make-dirs:
	if [ ! -d obj ]; then mkdir obj; fi
	if [ ! -d bin ]; then mkdir bin; fi

bin/snake-game: obj/snake-game.o obj/snake.o obj/simpcg.o obj/pixel-buffer.o
	$(CC) -o $@ $^ $(LIBS)

obj/snake-game.o: src/snake-game.c $(DEPS)
	$(CC) -c -o $@ $<

obj/snake.o: src/snake/snake.c $(DEPS)
	$(CC) -c -o $@ $<

obj/simpcg.o: src/simpcg/simpcg.c $(DEPS)
	$(CC) -c -o $@ $<

obj/pixel-buffer.o: src/simpcg/pixel-buffer.c $(DEPS)
	$(CC) -c -o $@ $<

clean:
	rm -rf bin/* obj/*

