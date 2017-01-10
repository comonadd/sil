###
### SIL Example Makefile
###

CURR_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

SRC = $(CURR_DIR)/main.c

all: $(EXAMPLE_PATH)

$(EXAMPLE_PATH): $(SRC)
	@echo -e $(YELLOW_CLR) "CC " $(GRAY_CLR) $@ $(RESET_CLR)
	@$(CC) $(CCFLAGS) $^ -L$(OUTPUT_DIR) -lsil -o $@
