TARGET = cecs
TEST_TARGET = check
LIBS = -lm -D_REENTRANT -std=c11 -lGL -lGLEW -lSDL2 -lSDL2_image

CC = gcc
CFLAGS = -g -Wall -I/usr/include/SDL2 -Isystems/ -Icomponents/ -Isrc/

.PHONY: default all clean FORCE

default: all
all: $(TARGET) $(TEST_TARGET)

ORIG_OBJECTS = $(patsubst src/%.c, src/%.o, $(wildcard src/**/*.c))
ORIG_OBJECTS += $(patsubst src/%.c, src/%.o, $(wildcard src/*.c))

OBJECTS := $(filter-out src/main.o,$(ORIG_OBJECTS))
HEADERS = $(wildcard src/*.h) $(wildcard src/**/*.h)
SRCS = $(wildcard src/*.c) $(wildcard src/**/*.c)

TEST_OBJECTS = $(patsubst tests/%.c, tests/%.o, $(wildcard tests/*.c))
TEST_HEADERS = $(wildcard tests/*.h) $(wildcard tests/**/*.h)
TEST_SRCS = $(wildcard tests/*.c) $(wildcard tests/**/*.c)

.PRECIOUS: $(TARGET) $(ORIG_OBJECTS) $(TEST_OBJECTS)

$(TARGET): $(OBJECTS) src/main.o
	@echo "========== Building $(TARGET) =========="
	$(CC) $(OBJECTS) src/main.o $(CFLAGS) $(LIBS) -o $@

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS) FORCE
	@echo "========== Building $(TEST_TARGET) =========="
	$(CC) $(CFLAGS) $(OBJECTS) $(TEST_OBJECTS) $(LIBS) -lcheck -o $@
	@echo "========== RUNNING TESTS =========="
	./$(TEST_TARGET)
	@echo ""

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

tests/%o: tests/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


clean:
	rm -f src/*.o
	rm -f tests/*.o
	rm -f $(TARGET)
	rm -f $(TEST_TARGET)

FORCE:


output:
	@echo "==== $(TARGET) ===="
	@echo "sources: $(SRCS)"
	@echo "headers: $(HEADERS)"
	@echo "objects: $(OBJECTS)"
	@echo "==== $(TEST_TARGET) ===="
	@echo "sources: $(TEST_SRCS)"
	@echo "headers: $(TEST_HEADERS)"
	@echo "objects: $(TEST_OBJECTS)"
