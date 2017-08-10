
TESTBINS=$(addprefix bin/,$(TESTS))

CFLAGS=-Wall -pedantic -O2 -std=gnu99 -DMALLOC_CHECK_=3 -g

# Sorting include paths and defines simplifies checking them
INCPATHS=. util base
INCLUDES=$(addprefix -I$(SRCDIR)/,$(INCPATHS))
DEPS=$(addprefix $(SRCDIR)/,$(addsuffix /*.h,$(INCPATHS)))

#OBJFILES=log.o linked_list.o array_list.o mem.o hashmap.o tokenizer.o infiles.o block.o cpu.o context.o errors.o segment.o operation.o parser.o arith.o print.o
OBJ=$(addprefix $(SRCDIR)/obj/,$(OBJFILES))

all: subdirs bin $(TESTBINS) 

bin: 
	mkdir bin

subdirs:
	if test x"$(SUBDIRS)" != "x"; then for i in $(SUBDIRS); do (cd $$i; make); done; fi

bin/%: %.c $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJ) $< 
	./$@ > $(notdir $@.x)
	cmp `echo "$@" | sed -e 's%bin/%%g'`.out $(notdir $@.x)
	rm $(notdir $@.x)

%: bin/%
	# not empty

clean:
	if test x"$(SUBDIRS)" != "x"; then for i in $(SUBDIRS); do (cd $$i; make clean); done; fi
	rm -f *.o $(TESTBINS)
	rmdir bin

 

