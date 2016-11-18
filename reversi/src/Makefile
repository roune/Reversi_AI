EXE=reversi

CFLAGS=-std=c99 -Wall -Wextra -g

.PHONY: all clean help

all: $(EXE)

$(EXE):	reversi.o bitboard.o
	gcc $(CFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $<

clean:
	@rm -f *~ *.o $(EXE)

help:
	@echo "all: run the whole build of reversi"
	@echo "reversi: builds from reversi.c and bitboard.c"
	@echo "clean: remove all files produced by compilation"
