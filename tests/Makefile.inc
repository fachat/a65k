
.SECONDARY:

SRCDIR=$(TESTROOT)/../src

TESTLIB=dbgprint.o
TESTLIBOBJ=$(addprefix $(TESTROOT)/,$(TESTLIB))

TESTBINS=$(addprefix bin/,$(TESTS))

CFLAGS=-g -Wall -pedantic -O2 -std=c11 -DMALLOC_CHECK_=3 

# Sorting include paths and defines simplifies checking them
INCPATHS=. util base 
INCLUDES=$(addprefix -I$(SRCDIR)/,$(INCPATHS)) -I$(TESTROOT)
DEPS=$(addprefix $(SRCDIR)/,$(addsuffix /*.h,$(INCPATHS)))

OBJ=$(addprefix $(SRCDIR)/obj/,$(OBJFILES))

# Enable verbose compile with make V=1
ifeq ($(V),1)
  V1=
  V2=@echo
  DIFF=@cmp
else
  V1=@
  V2=@\#
  DIFF=@diff -u
endif

all: $(TARGET) subdirs bin $(TESTS) 

bin: 
	mkdir bin

subdirs:
	$(V1) if test x"$(SUBDIRS)" != "x"; then for i in $(SUBDIRS); do echo "dir: "$$i; (cd $$i; make); done; fi

.SUFFIXES:

bin/%: %.c $(OBJ) $(TESTLIBOBJ) $(DEPS) bin
	@echo $<
	$(V1) $(CC) $(CFLAGS) $(INCLUDES) -o $@ $(OBJ) $(TESTLIBOBJ) $< 

%: bin/%
	$(V1) ./bin/$@ > $@.x
	$(V2) $(DIFF) $@.out $@.x
	$(DIFF) $@.out $@.x
	$(V1) rm $(notdir $@.x)

clean:
	$(V1) if test x"$(SUBDIRS)" != "x"; then for i in $(SUBDIRS); do echo "dir: "$$i; (cd $$i; make clean); done; fi
	rm -f *.o $(TESTBINS)
	sh -c "test -d bin && rmdir bin; return 0"

dbgprint.o: dbgprint.c 
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

