###
### SIL Examples Makefile
###

SRC := $(shell find . -type f -name '*.c')
OBJ := $(SRC:.c=.o)

BASIC_EXAMPLE_SRC := basic.c
KEYBINDINGS_EXAMPLE_SRC := keybindings.c
COMPLETIONS_EXAMPLE_SRC := completions.c

all: $(EXAMPLES)

$(BASIC_EXAMPLE_NAME): $(LIB_NAME) $(BASIC_EXAMPLE_SRC)
	$(CC) $(CCFLAGS) $^ -L$(OUTPUT_DIR) -lsil -o $@

$(KEYBINDINGS_EXAMPLE_NAME): $(LIB_NAME) $(KEYBINDINGS_EXAMPLE_SRC)
	$(CC) $(CCFLAGS) $^ -L$(OUTPUT_DIR) -lsil -o $@

$(COMPLETIONS_EXAMPLE_NAME): $(LIB_NAME) $(COMPLETIONS_EXAMPLE_SRC)
	$(CC) $(CCFLAGS) $^ -L$(OUTPUT_DIR) -lsil -o $@
