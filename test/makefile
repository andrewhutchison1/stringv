CC=clang
CFLAGS=-Wall -Wextra -Weverything -pedantic -std=c89 -m64 -Wshadow \
	   -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
	   -Wcast-align -Wswitch-default -Wbad-function-cast -Winline -Wundef \
	   -Wnested-externs -Wwrite-strings -Wconversion -Wunreachable-code \
	   -fno-common -fstrict-aliasing -Werror -Wstrict-overflow=5
LDFLAGS=

OBJDIR=obj
BINDIR=bin

TESTS=test_init test_clear
TESTBIN=$(addprefix $(BINDIR)/,$(TESTS))
TESTDEPS=$(addprefix $(OBJDIR)/,stringv.o test.o)

# Compile the stringv module
$(OBJDIR)/stringv.o: ../stringv.c ../stringv.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the test harness
$(OBJDIR)/test.o: test.c test.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compiles the tests
$(OBJDIR)/%.o: %.c test.h
	$(CC) $(CFLAGS) -c $< -o $@

# Links each test with the harness
$(TESTBIN): $(BINDIR)/%: $(OBJDIR)/%.o $(TESTDEPS)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: all
all: $(TESTBIN)

.PHONY: clean
clean:
	rm -rf ./$(OBJDIR)/* ./$(BINDIR)/*
