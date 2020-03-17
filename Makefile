####################
#   OS DETECTION   #
####################
# ensuring we have defaults for TARGET_OS and TARGET_NAME
ifndef $(TARGET_OS)
TARGET_OS := linux
endif
ifndef $(TARGET_NAME)
TARGET_NAME := cecs
endif

# checking for env. variables
ifeq ($(TARGET_OS), linux)
EXE_END :=
SO_END := .so
STO_END := .a
CC := gcc
endif
ifeq ($(TARGET_OS), darwin)
EXE_END :=
SO_END := .dylib
STO_END := .a
CC := clang
endif
ifeq ($(TARGET_OS), windows)
$(error windows builds are currently unsupported)
EXE_END := .exe
SO_END := .dll
STO_END := .lib
CC := gcc
endif

####################
#    TARGET VARS   #
####################
# exporting sane defaults for our various targets
export TARGET := lib$(TARGET_NAME)$(STO_END)
export TEST_TARGET := check
export COMP_TARGET := components$(EXE_END)
export EXAMPLE_SYSTEMS := lib$(TARGET_NAME)_sysfuncs$(SO_END)

MODULES := tests comp_gen
SYS_FUNC_DIR := example_sysfuncs

default: all
all: $(EXAMPLE_SYSTEMS) $(COMP_TARGET) $(TARGET) $(TEST_TARGET)

# base cflags, libs and srcs, used by ecs module
## look for include files in each of the modules
CFLAGS += $(patsubst %,-Isrc/%, $(MODULES)) -Isrc -D_REENTRANT -g -Wall
LIBS := -L/usr/lib -lyaml -ldl
SRC :=

# extra cflags, libs and srcs for each of our modules/submodules
## TEST_TARGET
TCFLAGS :=
TLIBS :=
TSRC :=

## COMP_TARGET
CCFLAGS :=
CLIBS :=
CSRC :=

## EXAMPLE_SYSTEMS
SOCFLAGS := -c -fPIC
SOLIBS := -ldl
### system funcs sources
SCFLAGS := $(patsubst %, -Isrc/%, $(SYS_FUNC_DIR))
SSRC :=

ECS_MDEFS := -DCECS_SYS_FUNCS=$(EXAMPLE_SYSTEMS)

# module code directories, required by submodules
export TESTS_DIR=src/tests
export CECS_DIR=src
export SYSFNCS_DIR=$(SYS_FUNC_DIR)
export COMP_GEN_DIR = src/comp_gen

# include the description for each module
include module.mk
include $(patsubst %, src/%/module.mk, $(MODULES))
include $(patsubst %, %/module.mk, $(SYS_FUNC_DIR))

# determining our object files
OBJ := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
TOBJ := $(patsubst %.c, %.o, $(filter %.c, $(TSRC)))
COBJ := $(patsubst %.c, %.o, $(filter %.c, $(CSRC)))
SOBJ := $(patsubst %.c, %.o, $(filter %.c, $(SSRC)))

# include the C include deps
include $(OBJ:.o=.d)

# setting additional flags for recipe targets
$(TEST_TARGET): LIBS += $(TLIB)
$(TEST_TARGET): OBJ := $(filter-out %/main.o, $(OBJ))
$(COMP_TARGET): LIBS += $(CLIB)
$(EXAMPLE_SYSTEMS): LIBS += $(SOLIBS)

$(OBJ): CFLAGS += $(ECS_MDEFS)
$(TOBJ): CFLAGS += $(TCFLAGS)
$(COBJ): CFLAGS += $(CCFLAGS)
$(SOBJ): CFLAGS += $(SCFLAGS) $(SOCFLAGS)

clean: OBJ += $(TOBJ) $(COBJ) $(UOBJ) $(SOBJ)
clean: TARGET += $(TEST_TARGET) $(COMP_TARGET) $(EXAMPLE_SYSTEMS)

####################
#     RECIPIES     #
####################
# link recipes
$(TARGET): $(OBJ) $(EXAMPLE_SYSTEMS) $(COMP_TARGET)
	@echo "========== building $(TARGET) =========="
	ar rcs $(TARGET) $(OBJ)
	-@ rm -f $(COMP_TARGET)

$(TEST_TARGET): $(OBJ) $(TOBJ) $(TARGET)
	@echo "========== building $(TEST_TARGET) =========="
	$(CC) -o $@ $(OBJ) $(TOBJ) $(LIBS)
	-LD_LIBRARY_PATH=. ./$(TEST_TARGET)

$(COMP_TARGET): $(COBJ)
	@echo "========== building $(COMP_TARGET) =========="
	$(CC) -o $@ $(COBJ) $(LIBS)
	@./$(COMP_TARGET) components.yml $(CECS_DIR)/components components

$(EXAMPLE_SYSTEMS): $(SOBJ)
	@echo "========== building $(EXAMPLE_SYSTEMS) =========="
ifneq ($(SOBJ),)
	@echo "========== building $(EXAMPLE_SYSTEMS) =========="
ifeq ($(TARGET_OS),linux)
	$(CC) -shared -Wl,-soname,${EXAMPLE_SYSTEMS},-rpath=. -o $(EXAMPLE_SYSTEMS) $(SOBJ) $(SLIB)
else ifeq ($(TARGET_OS),darwin)
	$(CC) -dynamiclib -install_name $(EXAMPLE_SYSTEMS) -o $(EXAMPLE_SYSTEMS) $(SOBJ) $(SLIB)
else ifeq ($(TARGET_OS),windows)
	@echo "Windows builds are currently unsupported"
endif
endif


# calculate the C include deps
%.d: %.c
	./depend.sh `dirname $*.c` $(CFLAGS) $*.c > $@

clean:
	-rm -f $(TARGET)
	-find . -name '*.o' -exec rm {} \;
	-find . -name '*.d' -exec rm {} \;
	-find . -name 'components.c' -exec rm {} \;
	-find . -name 'components.h' -exec rm {} \;

output:
	@echo "========== $(TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(ECS_MDEFS)"
	@echo "LIBS:"
	@echo "    $(LIBS)"
	@echo "OBJ:"
	@echo "    $(OBJ)"
	@echo "========== $(TEST_TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(TCFLAGS)"
	@echo "LIBS:"
	@echo "    $(LIBS) $(TLIB)"
	@echo "OBJ:"
	@echo "    $(OBJ) $(TOBJ)"
	@echo "========== $(COMP_TARGET) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(CCFLAGS)"
	@echo "LIBS:"
	@echo "    $(LIBS) $(CLIB)"
	@echo "OBJ:"
	@echo "    $(COBJ)"
	@echo "========== $(EXAMPLE_SYSTEMS) =========="
	@echo "CFLAGS:"
	@echo "    $(CFLAGS) $(SOFLAGS) $(SCFLAGS)"
	@echo "LIBS:"
	@echo "    $(LIBS) $(SLIB)"
	@echo "OBJ:"
	@echo "    $(SOBJ)"

.PHONY: default all clean FORCE
