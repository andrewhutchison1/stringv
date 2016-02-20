CC 		=clang
CFLAGS	=-std=c89 -Wall -Wextra -Weverything -pedantic
CPPFLAGS=
TESTCPPFLAGS=-DTEST_SHOW_SUCCESS=1
LDFLAGS	=

obj/stringv.o: ../stringv.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -O3 -c ../stringv.c -o obj/stringv.o

test_init: obj/stringv.o test_init.c
	$(CC) $(CFLAGS) $(TESTCPPFLAGS) test_init.c obj/stringv.o -o bin/test_init

all: obj/stringv.o test_init
