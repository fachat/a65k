
TESTBINS=$(addprefix bin/,$(TESTS))

CFLAGS=-Wall -pedantic -O2 -std=gnu99 -DMALLOC_CHECK_=3 -g

# Sorting include paths and defines simplifies checking them
INCPATHS=. util base
INCLUDES=$(addprefix -I$(SRCDIR)/,$(INCPATHS))
DEPS=$(addprefix $(SRCDIR)/,$(addsuffix /*.h,$(INCPATHS)))

OBJ=$(addprefix $(SRCDIR)/obj/,$(OBJFILES))

# Enable verbose compile with make V=1
ifeq ($(V),1)
  V1=
  V2=@\#
  DIFF=@cmp
else
  V1=@
  V2=@echo
  DIFF=@diff -u
endif

all: subdirs bin $(TESTBINS) 

bin: 
	mkdir bin

subdirs:
	$(V1) if test x"$(SUBDIRS)" != "x"; then for i in $(SUBDIRS); do echo "dir: "$$i; (cd $$i; make); done; fi

bin/%: %.c $(OBJ) $(DEPS)
	$(V2) $<
	$(V1) $(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJ) $< 
	$(V1) ./$@ > $(notdir $@.x)
	$(DIFF) `echo "$@" | sed -e 's%bin/%%g'`.out $(notdir $@.x)
	$(V1) rm $(notdir $@.x)

%: bin/%
	$(V1) ./bin/$@ > $@.x
	$(V2) $(DIFF) $@.out $@.x
	$(DIFF) $@.out $@.x
	$(V1) rm $(notdir $@.x)

clean:
	$(V1) if test x"$(SUBDIRS)" != "x"; then for i in $(SUBDIRS); do echo "dir: "$$i; (cd $$i; make clean); done; fi
	rm -f *.o $(TESTBINS)
	rmdir bin

 

