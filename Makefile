PROGRAM=steg

# Directory structure
IDIR = include
ODIR = obj
SRCDIR = src
BINDIR = bin

# Compiler and C flags
CC=gcc
CFLAGS=-I$(IDIR)

# Libraries
LIBS = 

# Dependencies and obj files
_DEPS = lodepng.h
_OBJ = $(PROGRAM).o lodepng.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Create executable
$(BINDIR)/$(PROGRAM): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~


