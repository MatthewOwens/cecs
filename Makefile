TARGET = libcecs.a
TEST_TARGET = check

LIBS = -lm -D_REENTRANT -std=c11
TEST_LIBS = $(LIBS) `pkg-config --libs check`

CC = gcc
CFLAGS = -g -Wall -Isystems/ -Icomponents/ -Isrc/ -I/usr/local/include
TEST_CFLAGS = $(CFLAGS) `pkg-config --cflags check`

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

.PRECIOUS: $(TARGET) $(TEST_TARGET)

#$(LIB_TEST_TARGET): src/main.c $(TARGET)
#	$(CC) $(CFLAGS) -o $@ $^ -L. -lcecs

$(TARGET): $(OBJECTS)
	@echo "========== Building $(TARGET) =========="
	ar rcs libcecs.a $(OBJECTS)

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS) FORCE
	@echo "========== Building $(TEST_TARGET) =========="
	$(CC) $(TEST_CFLAGS) $(OBJECTS) $(TEST_OBJECTS) $(TEST_LIBS) -o $@
	@echo "========== RUNNING TESTS =========="
	./$(TEST_TARGET)
	@echo ""

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

tests/%o: tests/%.c
	$(CC) $(TEST_CFLAGS) -c $^ -o $@

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
