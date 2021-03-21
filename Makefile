CC     = gcc
OPTIMIZATION = -O3
CFLAGS = -Wall -Wextra -Wpedantic -std=c99
LIBS   = -pthread

DEPS = src/snake/snake.h \
       src/simpcg/simpcg.h \
       src/simpcg/pixel-buffer.h \
       src/option-map/option-map.h \
       $(DEBUG_DEPS)

OBJS = obj/snake-game.o \
       obj/snake.o \
       obj/simpcg.o \
       obj/pixel-buffer.o \
       obj/option-map.o \
       $(DEBUG_OBJS)

DEFINES = -DNDEBUG

all: make-dirs bin/snake-game

debug:
	make all DEBUG_DEPS=src/mem-debug/mem-debug.h DEBUG_OBJS=obj/mem-debug.o OPTIMIZATION=-g DEFINES=-DMEM_DEBUG

make-dirs: obj/ bin/

obj/:
	if [ ! -d obj ]; then mkdir obj; fi

bin/:
	if [ ! -d bin ]; then mkdir bin; fi

# snake-game

bin/snake-game: $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(DEFINES)

obj/snake-game.o: src/snake-game.c $(DEPS)
	$(CC) -c -o $@ $< $(DEFINES)

obj/snake.o: src/snake/snake.c $(DEPS)
	$(CC) -c -o $@ $< $(DEFINES)

# simpcg

obj/simpcg.o: src/simpcg/simpcg.c $(DEPS)
	$(CC) -c -o $@ $< $(DEFINES)

obj/pixel-buffer.o: src/simpcg/pixel-buffer.c $(DEPS)
	$(CC) -c -o $@ $< $(DEFINES)

# option-map
obj/option-map.o: src/option-map/option-map.c $(DEPS)
	$(CC) -c -o $@ $< $(DEFINES)

# mem-debug
obj/mem-debug.o: src/mem-debug/mem-debug.c $(DEPS)
	$(CC) -c -o $@ $<

clean:
	rm -rf obj/*

.PHONY: release debug make-dirs clean

