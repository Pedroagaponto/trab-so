CC = gcc
#CC = clang

SDIR = src
ODIR = obj
TDIR = tests

CFLAGS = -Wall -Wextra -g

BIN = execproc execprocd
TESTS = pi

all: tests bin
bin: $(BIN)
tests: $(TESTS)

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

#Executaveis solicitados
execproc: $(ODIR)/execproc.o
	$(CC) -o $@ $^ $(CFLAGS)

execprocd: $(ODIR)/execprocd.o
	$(CC) -o $@ $^ $(CFLAGS)

#Executaveis de teste para o execproc
pi: $(ODIR)/pi.o
	@mkdir -p $(TDIR)
	$(CC) -o $(TDIR)/$@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o  a.out $(BIN) $(patsubst %,$(TDIR)/%,$(TESTS))
	-rmdir $(ODIR)
	-rmdir $(TDIR)
