###
### SIL Main Makefile
###

SRC := $(shell find . -type f -name '*.c')
OBJ := $(SRC:.c=.o)

all: $(LIB_NAME)

$(LIB_NAME): $(OBJ)
	@echo -e $(YELLOW_CLR) "AR " $(GRAY_CLR) $@ $(RESET_CLR)
	@ar -rcs $@ $^

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
