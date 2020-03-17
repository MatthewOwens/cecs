CFLAGS += -I$(CECS_DIR)/core -I$(CECS_DIR)/entities -I$(CECS_DIR)/components -I$(CECS_DIR)/systems

SRC += $(wildcard $(CECS_DIR)/core/*.c)
SRC += $(wildcard $(CECS_DIR)/entities/*.c)
SRC += $(wildcard $(CECS_DIR)/components/*.c)
SRC += $(wildcard $(CECS_DIR)/systems/*.c)
