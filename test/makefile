CC=clang
CPPFLAGS=-DNDEBUG
CFLAGS=-std=c99 -pedantic -m64 -fno-common -fstrict-aliasing @warnings -ggdb3
LDFLAGS=

OBJDIR=obj
BINDIR=bin

TESTS=test_init test_clear test_copy test_push_back test_push_front \
	  test_get test_insert test_remove test_iteration
TESTBIN=$(addprefix $(BINDIR)/,$(TESTS))
TESTDEPS=$(addprefix $(OBJDIR)/,stringv.o test.o)

# Compile the stringv module
$(OBJDIR)/stringv.o: ../stringv.c ../stringv.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

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
	rm -f ./$(OBJDIR)/* ./$(BINDIR)/*
