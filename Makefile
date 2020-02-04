TARGET = libcecs.a
TEST_TARGET = check
COMP_TARGET = components

LIBS = -lm -D_REENTRANT -std=c11 -lyaml -lcyaml
TEST_LIBS = $(LIBS) `pkg-config --libs check`

CC = clang
CFLAGS = -g -Wall -Isystems/ -Icomponents/ -Isrc/ -I/usr/local/include -v
TEST_CFLAGS = $(CFLAGS) `pkg-config --cflags check`

.PHONY: default all clean FORCE

default: all
all: $(TARGET) $(TEST_TARGET)

ORIG_OBJECTS = $(patsubst src/%.c, src/%.o, $(wildcard src/**/*.c))
ORIG_OBJECTS += $(patsubst src/%.c, src/%.o, $(wildcard src/*.c))
COMP_OBJECTS = src/comp_gen.o src/yaml_helper.o

# filtering out main so we can use the same var for our tests
# and the component generator, used in it's own target
OBJECTS := $(filter-out src/main.o src/comp_gen.o ,$(ORIG_OBJECTS))

HEADERS = $(wildcard src/*.h) $(wildcard src/**/*.h)
SRCS = $(wildcard src/*.c) $(wildcard src/**/*.c)

TEST_OBJECTS = $(patsubst tests/%.c, tests/%.o, $(wildcard tests/*.c))
TEST_HEADERS = $(wildcard tests/*.h) $(wildcard tests/**/*.h)
TEST_SRCS = $(wildcard tests/*.c) $(wildcard tests/**/*.c)

.PRECIOUS: $(TARGET) $(TEST_TARGET)

$(COMP_TARGET): $(COMP_OBJECTS)
	$(CC) $(CFLAGS) $(COMP_OBJECTS) $(LIBS) -o $@
	-./$(COMP_TARGET) components.yml src components
	rm -f $(COMP_TARGET)

$(TARGET): $(COMP_OBJECTS) $(OBJECTS) 
	@echo "========== BUILDING CECS $(TARGET) =========="
	ar rcs libcecs.a $(OBJECTS)

$(TEST_TARGET): $(COMP_OBJECTS) $(OBJECTS) $(TEST_OBJECTS) FORCE
	@echo "========== BUILDING CECS $(TEST_TARGET) =========="
	$(CC) $(TEST_CFLAGS) $(OBJECTS) $(TEST_OBJECTS) $(TEST_LIBS) -o $@
	@echo "========== RUNNING CECS TESTS =========="
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
	rm -f $(COMP_TARGET)
	rm -f $(TEST_TARGET)
	rm -f src/components.*

FORCE:


output:
	@echo "==== $(COMP_TARGET) ===="
	@echo "object: $(COMP_OBJECTS) ===="
	@echo "==== $(TARGET) ===="
	@echo "sources: $(SRCS)"
	@echo "headers: $(HEADERS)"
	@echo "objects: $(OBJECTS)"
	@echo "==== $(TEST_TARGET) ===="
	@echo "sources: $(TEST_SRCS)"
	@echo "headers: $(TEST_HEADERS)"
	@echo "objects: $(TEST_OBJECTS)"
