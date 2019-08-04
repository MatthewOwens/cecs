TARGET = cecs
LIBS = -lm -D_REENTRANT -std=c11 -lGL -lGLEW -lSDL2 -lSDL2_image

CC = gcc
CFLAGS = -g -Wall -I/usr/include/SDL2 -Isystems/ -Icomponents/ -Isrc/

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst src/%.c, src/%.o, $(shell find . -name '*.c'))
HEADERS = $(wildcard src/*.h) $(wildcard src/**/*.h)
SRCS = $(wildcard src/*.c) $(wildcard src/**/*.c)

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
		$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

src/%.o: src/%.c
		$(CC) $(CFLAGS) -c $^ -o $@

clean:
		rm -f src/*.o
		rm -f $(TARGET)

output:
		@echo "==== make ===="
		@echo "sources: $(SRCS)"
		@echo "headers: $(HEADERS)"
		@echo "objects: $(OBJECTS)"
