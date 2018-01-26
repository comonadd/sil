###
### SIL Main Makefile
###

SRC := $(shell find . -type f -name '*.c')
OBJ := $(SRC:.c=.o)
OBJ_FPIC := $(OBJ:.o=.o.fpic)

all: $(LIB_SHARED_NAME) $(LIB_STATIC_NAME)

$(LIB_SHARED_NAME): $(OBJ_FPIC)
	@echo -e $(YELLOW_CLR) "LD " $(GRAY_CLR) $@ $(RESET_CLR)
	@$(CC) $(CCFLAGS) -o $@ -shared $^

$(LIB_STATIC_NAME): $(OBJ)
	@echo -e $(YELLOW_CLR) "AR " $(GRAY_CLR) $@ $(RESET_CLR)
	@$(AR) rcs $@ $^

%.o %.o.fpic: %.c
	@echo -e $(YELLOW_CLR) "CC " $(GRAY_CLR) $*.c $(RESET_CLR)
	@$(CC) -c -fpic $(CCFLAGS) -o $*.o.fpic $*.c
	@$(CC) -c $(CCFLAGS) -o $*.o $*.c
	@$(CC) -MM $(CCFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

-include $(OBJ:.o=.d)
