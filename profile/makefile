CC=clang
CPPFLAGS=-DNDEBUG
CFLAGS=-std=c99 -pedantic -m64 -fno-common -O3
LDFLAGS=

OBJDIR=obj
BINDIR=bin

PROFILES=profile_iteration
PROFILEBIN=$(addprefix $(BINDIR)/,$(PROFILES))
PROFILEDEPS=$(addprefix $(OBJDIR)/,stringv.o profile.o)

# Compile the stringv module
$(OBJDIR)/stringv.o: ../stringv.c ../stringv.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Compile the profile object
$(OBJDIR)/profile.o: profile.c profile.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compiles the profiles
$(OBJDIR)/%.o: %.c profile.h
	$(CC) $(CFLAGS) -c $< -o $@

# Links each profile with the profile object
$(PROFILEBIN): $(BINDIR)/%: $(OBJDIR)/%.o $(PROFILEDEPS)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: all
all: $(PROFILEBIN)

.PHONY: clean
clean:
	rm -f ./$(OBJDIR)/* ./$(BINDIR)/*
