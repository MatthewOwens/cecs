TLIB += `pkg-config --libs check`
TCFLAGS += -I$(TESTS_DIR) -I$(TESTS_DIR)/ecs -I$(TESTS_DIR)/engine
TCFLAGS += `pkg-config --cflags check`
TSRC += $(wildcard $(TESTS_DIR)/*.c)
