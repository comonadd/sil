###
### SIL Examples Makefile
###

SRC := $(shell find . -type f -name '*.c')
OBJ := $(SRC:.c=.o)

EXAMPLES := \
	$(BASIC_EXAMPLE_NAME)
	# $(KEYBINDINGS_EXAMPLE_NAME) \
	# $(COMPLETIONS_EXAMPLE_NAME)

BASIC_EXAMPLE_OBJ := example_basic.o
# KEYBINDINGS_EXAMPLE_OBJ := example_keybindings.o
# COMPLETIONS_EXAMPLE_OBJ := example_completions.o

all: $(LIB_NAME) $(EXAMPLES)

$(BASIC_EXAMPLE_NAME): $(BASIC_EXAMPLE_OBJ)
	@echo -e $(YELLOW_CLR) "LD " $(GRAY_CLR) $@ $(RESET_CLR)
	@$(CC) $(CCFLAGS) $^ $(LIB_STATIC_NAME) -o $@

# $(KEYBINDINGS_EXAMPLE_NAME): $(KEYBINDINGS_EXAMPLE_OBJ)
#	@echo -e $(YELLOW_CLR) "LD " $(GRAY_CLR) $@ $(RESET_CLR)
#	@$(LD) $(LDFLAGS) $^ $(LIB_STATIC_NAME) -o $@

# $(COMPLETIONS_EXAMPLE_NAME): $(COMPLETIONS_EXAMPLE_OBJ)
#	@echo -e $(YELLOW_CLR) "LD " $(GRAY_CLR) $@ $(RESET_CLR)
#	@$(LD) $(LDFLAGS) $^ $(LIB_STATIC_NAME) -o $@

%.o: %.c
	@echo -e $(YELLOW_CLR) "CC " $(GRAY_CLR) $*.c $(RESET_CLR)
	@$(CC) -c $(CCFLAGS) $*.c -o $*.o
	@$(CC) -MM $(CCFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

-include $(OBJ:.o=.d)
