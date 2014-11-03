CC = gcc

SDIR = src
ODIR = obj

_OBJ = execproc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS = -Wall -Wextra -g

BIN = execproc

all: $(BIN)

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

execproc: $(ODIR)/execproc.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o  a.out $(BIN)
	rmdir $(ODIR)
