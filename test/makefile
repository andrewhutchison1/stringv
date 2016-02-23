CC=clang
CFLAGS=-Wall -Wextra -Weverything -pedantic -std=c89 -m64 -Wshadow \
	   -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
	   -Wcast-align -Wswitch-default -Wbad-function-cast -Winline -Wundef \
	   -Wnested-externs -Wwrite-strings -Wconversion -Wunreachable-code \
	   -fno-common -fstrict-aliasing -Werror -Wstrict-overflow=5
OBJDIR=obj
BINDIR=bin
TESTDEPS=$(OBJDIR)/stringv.o $(OBJDIR)/test.o

$(OBJDIR)/stringv.o: ../stringv.c
	$(CC) $(CFLAGS) -O2 -c $< -o $@

$(OBJDIR)/test.o: test.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/test_init: test_init.c $(OBJDIR)/stringv.o $(OBJDIR)/test.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: all
all: $(BINDIR)/test_init

.PHONY: clean
clean:
	rm -rf ./$(OBJDIR)/* ./$(BINDIR)/*
