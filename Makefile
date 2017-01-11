###
### SIL Root Makefile
###

YELLOW_CLR ?= "\e[38;5;226m"
GRAY_CLR ?= "\e[38;5;239m"
RESET_CLR ?= "\e[0m"

ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
OUTPUT_DIR := $(ROOT_DIR)/tmp

TEST := 1
DEBUG := 1

####################
### Linker setup ###
####################

LD := gcc
LDFLAGS := -lc

########################
### C Compiler setup ###
########################

CC := gcc
CCSTD := -std=gnu99 -pedantic
CCWARN_FLAGS := \
	-Wno-missing-braces -Wextra -Wno-missing-field-initializers -Wformat=2 \
	-Wswitch-default -Wcast-align -Wpointer-arith \
	-Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline \
	-Wundef -Wnested-externs -Wshadow -Wunreachable-code \
	-Wlogical-op -Wfloat-equal -Wstrict-aliasing=2 -Wredundant-decls \
	-Wold-style-definition -Werror
CCOPTIMIZATION_FLAGS := -O0
CCDEFINITIONS := \
	-DTEST=$(TEST) -DDEBUG=$(DEBUG)
CCINCLUDES := -I$(ROOT_DIR)/src/

CCFLAGS := $(CCSTD) $(CCWARN_FLAGS) $(CCOPTIMIZATION_FLAGS) $(CCDEFINITIONS) $(CCINCLUDES) \
	-fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing \
	-lm

ifeq ($(DEBUG), 1)
CCFLAGS += -g -ggdb
endif

#############
### Paths ###
#############

LIB_SRC_PATH := $(ROOT_DIR)/src
LIB_NAME := $(OUTPUT_DIR)/libsil.a

EXAMPLES_SRC_PATH := $(ROOT_DIR)/examples
BASIC_EXAMPLE_NAME := $(OUTPUT_DIR)/example_basic
KEYBINDINGS_EXAMPLE_NAME := $(OUTPUT_DIR)/example_keybindings
COMPLETIONS_EXAMPLE_NAME := $(OUTPUT_DIR)/example_completions

EXAMPLES := \
	$(BASIC_EXAMPLE_NAME) \
	$(KEYBINDINGS_EXAMPLE_NAME) \
	$(COMPLETIONS_EXAMPLE_NAME)

export

###############
### Targets ###
###############

all: library

library: $(LIB_NAME)
.PHONY: library

$(LIB_NAME): $(LIB_SRC_PATH)
	+$(MAKE) -C $^

examples: $(EXAMPLES) $(LIB_NAME)
.PHONY: examples

$(EXAMPLES): $(EXAMPLES_SRC_PATH) $(LIB_NAME)
	+$(MAKE) -C $(EXAMPLES_SRC_PATH)

clean:
	@find . -name "*.o" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.o" $(RESET_CLR)
	@find . -name "*.d" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.d" $(RESET_CLR)
	@find . -name "*.d.tmp" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.d.tmp" $(RESET_CLR)
	@find . -name "*.a" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.a" $(RESET_CLR)
	rm -rf $(BASIC_EXAMPLE_NAME) $(KEYBINDINGS_EXAMPLE_NAME) $(COMPLETIONS_EXAMPLE_NAME)
.PHONY: clean
