CCFLAGS += -I$(COMP_GEN_DIR)
CSRC += $(wildcard $(COMP_GEN_DIR)/*.c)
CSRC += $(CECS_DIR)/core/yaml_helper.c
