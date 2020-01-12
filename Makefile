TARGET = libcecs.a
TEST_TARGET = check

LIBS = -lm -D_REENTRANT -std=c11 -lyaml -lcyaml
TEST_LIBS = $(LIBS) `pkg-config --libs check`

CC = clang
CFLAGS = -g -Wall -L/usr/lib -Isystems/ -Icomponents/ -Isrc/ -I/usr/local/include
TEST_CFLAGS = $(CFLAGS) `pkg-config --cflags check`

.PHONY: default all clean FORCE

default: all
all: $(TARGET) $(TEST_TARGET)

ORIG_OBJECTS = $(patsubst src/%.c, src/%.o, $(wildcard src/**/*.c))
ORIG_OBJECTS += $(patsubst src/%.c, src/%.o, $(wildcard src/*.c))

# filtering out main so we can use the same var for our tests
# and the component generator, used in it's own target
OBJECTS := $(filter-out src/main.o src/comp_gen.o ,$(ORIG_OBJECTS))

HEADERS = $(wildcard src/*.h) $(wildcard src/**/*.h)
SRCS = $(wildcard src/*.c) $(wildcard src/**/*.c)

TEST_OBJECTS = $(patsubst tests/%.c, tests/%.o, $(wildcard tests/*.c))
TEST_HEADERS = $(wildcard tests/*.h) $(wildcard tests/**/*.h)
TEST_SRCS = $(wildcard tests/*.c) $(wildcard tests/**/*.c)

.PRECIOUS: $(TARGET) $(TEST_TARGET)

#$(LIB_TEST_TARGET): src/main.c $(TARGET)
#	$(CC) $(CFLAGS) -o $@ $^ -L. -lcecs

$(TARGET): $(OBJECTS) component_gen
	@echo "========== BUILDING CECS $(TARGET) =========="
	ar rcs libcecs.a $(OBJECTS)

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS) FORCE
	@echo "========== BUILDING CECS $(TEST_TARGET) =========="
	$(CC) $(TEST_CFLAGS) $(OBJECTS) $(TEST_OBJECTS) $(TEST_LIBS) -o $@
	@echo "========== RUNNING CECS TESTS =========="
	./$(TEST_TARGET)
	@echo ""

component_gen: src/comp_gen.o FORCE
	$(CC) $(CFLAGS) src/comp_gen.o $(LIBS) -o $@
	./component_gen components.yml src/components

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
