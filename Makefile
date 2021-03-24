CC     = gcc
OPTIMIZATION = -O3
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 $(OPTIMIZATION)
LIBS   = -pthread

DEPS = src/snake/snake.h \
       src/simpcg/simpcg.h \
       src/option-map/option-map.h \
       src/mem-utils/mem-macros.h \
       $(DEBUG_DEPS)

OBJS = obj/snake-game.o \
       obj/snake.o \
       obj/scg-buffer.o \
       obj/scg-pixel-buffer.o \
       obj/option-map.o \
       $(DEBUG_OBJS)

DEBUG = -DNDEBUG
DEFINES= -D_DEFAULT_SOURCE $(DEBUG)

all: make-dirs bin/snake-game

debug:
	make all DEBUG_DEPS=src/mem-debug/mem-debug.h DEBUG_OBJS=obj/mem-debug.o OPTIMIZATION=-g DEBUG=-DMEM_DEBUG

make-dirs: obj/ bin/

obj/:
	if [ ! -d obj ]; then mkdir obj; fi

bin/:
	if [ ! -d bin ]; then mkdir bin; fi

# snake-game

bin/snake-game: $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(DEFINES)

obj/snake-game.o: src/snake-game.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

obj/snake.o: src/snake/snake.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

# simpcg

obj/scg-buffer.o: src/simpcg/scg-buffer.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

obj/scg-pixel-buffer.o: src/simpcg/scg-pixel-buffer.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

# option-map
obj/option-map.o: src/option-map/option-map.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

# mem-debug
obj/mem-debug.o: src/mem-utils/mem-debug.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEFINES)

clean:
	rm -rf obj/*

.PHONY: release debug make-dirs clean

