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


LD := ld
LDFLAGS := -lc

########################
### C Compiler setup ###
########################

ifeq ($(DEBUG), 1)
CCDEBUG_FLAGS := -g -ggdb
CCOPTIMIZATION_FLAGS := -O0
endif

CC := gcc
CCSTD := -std=gnu99 -pedantic
CCWARN_FLAGS := \
	-Wno-missing-braces -Wextra -Wno-missing-field-initializers -Wformat=2 \
	-Wswitch-default -Wcast-align -Wpointer-arith \
	-Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline \
	-Wundef -Wnested-externs -Wshadow -Wunreachable-code \
	-Wlogical-op -Wfloat-equal -Wstrict-aliasing=2 -Wredundant-decls \
	-Wold-style-definition -Werror
CCOPTIMIZATION_FLAGS ?= -O2
CCDEFINITIONS := -DTEST=$(TEST) -DDEBUG=$(DEBUG)
CCINCLUDES := -I$(ROOT_DIR)/src

CCFLAGS := $(CCSTD) $(CCWARN_FLAGS) $(CCDEBUG_FLAGS) $(CCOPTIMIZATION_FLAGS) $(CCDEFINITIONS) $(CCINCLUDES) \
	-fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing

#############
### Paths ###
#############

LIB_SRC_PATH := $(ROOT_DIR)/src
LIB_SHARED_NAME := $(OUTPUT_DIR)/libsil.so
LIB_STATIC_NAME := $(OUTPUT_DIR)/libsil.a

BASIC_EXAMPLE_NAME := $(OUTPUT_DIR)/example_basic
KEYBINDINGS_EXAMPLE_NAME := $(OUTPUT_DIR)/example_keybindings
COMPLETIONS_EXAMPLE_NAME := $(OUTPUT_DIR)/example_completions

export

###############
### Targets ###
###############

all: library

library: $(OUTPUT_DIR)
	+$(MAKE) -C $(LIB_SRC_PATH)
.PHONY: library

examples: $(OUTPUT_DIR) library
	+$(MAKE) -C examples
.PHONY: examples

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

clean:
	@find . -name "*.o" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.o" $(RESET_CLR)
	@find . -name "*.o.fpic" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.o.fpic" $(RESET_CLR)
	@find . -name "*.d" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.d" $(RESET_CLR)
	@find . -name "*.d.tmp" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.d.tmp" $(RESET_CLR)
	@find . -name "*.a" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.a" $(RESET_CLR)
	@find . -name "*.so" -type f -delete
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "*.so" $(RESET_CLR)
	@rm -rf $(OUTPUT_DIR)
	@echo -e $(YELLOW_CLR) "CLEAN " $(GRAY_CLR) "$(OUTPUT_DIR)" $(RESET_CLR)
.PHONY: clean
