CC 		=clang
CFLAGS	=-std=c89 -Wall -Wextra -Weverything -pedantic
CPPFLAGS=
TESTCPPFLAGS=-DTEST_SHOW_SUCCESS=1
LDFLAGS	=
OUTDIR 	=bin
OBJDIR	=obj

$(OBJDIR)/stringv.o: ../stringv.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -c $< -o $@

test_init: $(OBJDIR)/stringv.o test_init.c
	$(CC) $(CFLAGS) $(TESTCPPFLAGS) $^ -o $(OUTDIR)/$@

test_copy: $(OBJDIR)/stringv.o test_copy.c
	$(CC) $(CFLAGS) $(TESTCPPFLAGS) $^ -o $(OUTDIR)/$@

all: obj/stringv.o test_init
