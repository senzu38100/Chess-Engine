all:
	gcc senzu.c init.c misc.c bitboards.c search.c perft.c makemove.c validate.c pvtable.c  hashkeys.c Board.c movegen.c attack.c io.c data.c evaluate.c uci.c xboard.c -o senzu

CCWIN=x86_64-w64-mingw32-gcc
CFLAGS=-O2 -Wall -std=c99

# Liste de tes fichiers source
SRCS=senzu.c init.c misc.c bitboards.c search.c perft.c makemove.c validate.c pvtable.c  hashkeys.c Board.c movegen.c attack.c io.c data.c evaluate.c uci.c 

OBJS=$(SRCS:.c=.o)

senzu.exe: $(SRCS)
	$(CCWIN) $(CFLAGS) -o senzu.exe $(SRCS)
